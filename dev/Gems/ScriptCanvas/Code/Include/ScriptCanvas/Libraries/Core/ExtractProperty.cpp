/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include <precompiled.h>
#include <ScriptCanvas/Libraries/Core/ExtractProperty.h>
#include <ScriptCanvas/Libraries/Core/BehaviorContextObjectNode.h>
#include <Libraries/Core/MethodUtility.h>

namespace ScriptCanvas
{
    namespace Nodes
    {
        namespace Core
        {
            void ExtractProperty::SlotMetadata::Reflect(AZ::ReflectContext* context)
            {
                if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
                {
                    serializeContext->Class<SlotMetadata>()
                        ->Field("m_slotId", &SlotMetadata::m_slotId)
                        ->Field("m_dataType", &SlotMetadata::m_dataType)
                        ;
                }
            }

            void ExtractProperty::OnInit()
            {
                m_sourceAccount.m_slotId = AddInputDatumDynamicTypedSlot("Source");
                EndpointNotificationBus::Handler::BusConnect({ GetEntityId(), m_sourceAccount.m_slotId });
                RefreshGetterFunctions();
            }
            
            Data::Type ExtractProperty::GetSourceSlotDataType() const
            {
                return m_sourceAccount.m_dataType;
            }

            void ExtractProperty::OnInputSignal(const SlotId& slotID)
            {
                if (slotID == GetSlotId(GetInputSlotName()))
                {
                    if (auto input = GetInput(m_sourceAccount.m_slotId))
                    {
                        for(auto&& propertyAccount : m_propertyAccounts)
                        {
                            Slot* propertySlot = GetSlot(propertyAccount.m_propertySlotId);
                            if (propertySlot && propertyAccount.m_getterFunction)
                            {
                                auto outputOutcome = propertyAccount.m_getterFunction(*input);
                                if (!outputOutcome)
                                {
                                    SCRIPTCANVAS_REPORT_ERROR((*this), outputOutcome.TakeError().data());
                                    return;
                                }
                                PushOutput(outputOutcome.TakeValue(), *propertySlot);
                            }
                        }
                    }
                    SignalOutput(GetSlotId(GetOutputSlotName()));
                }
            }

            bool ExtractProperty::SlotAcceptsType(const SlotId& slotID, const Data::Type& type) const
            {
                if (slotID == m_sourceAccount.m_slotId)
                {
                    if (!type.IsValid())
                    {
                        return false;
                    }
                    Slot* sourceSlot = GetSlot(m_sourceAccount.m_slotId);
                    return sourceSlot && DynamicSlotInputAcceptsType(sourceSlot->GetId(), type, Node::DynamicTypeArity::Single, *sourceSlot);
                }
                return true;
            }

            void ExtractProperty::OnEndpointConnected(const Endpoint& dataOutEndpoint)
            {
                AZ::Entity* dataOutEntity{};
                AZ::ComponentApplicationBus::BroadcastResult(dataOutEntity, &AZ::ComponentApplicationRequests::FindEntity, dataOutEndpoint.GetNodeId());
                auto dataOutNode = dataOutEntity ? AZ::EntityUtils::FindFirstDerivedComponent<Node>(dataOutEntity) : nullptr;
                if (dataOutNode)
                {
                    // Sets the source metadata data type to be the same as the endpoint
                    m_sourceAccount.m_dataType = dataOutNode->GetSlotDataType(dataOutEndpoint.GetSlotId());
                    ClearPropertySlots();
                    AddPropertySlots(m_sourceAccount.m_dataType);
                }
            }

            void ExtractProperty::OnEndpointDisconnected(const Endpoint& targetEndpoint)
            {
                ClearPropertySlots();
                m_sourceAccount.m_dataType = Data::Type::Invalid(); // Reset the source metadata data type to invalid on disconnect
            }

            void ExtractProperty::AddPropertySlots(const Data::Type& type)
            {
                Data::GetterContainer getterFunctions = Data::ExplodeToGetters(type);
                for (const auto& getterWrapperPair : getterFunctions)
                {
                    const AZStd::string& propertyName = getterWrapperPair.first;
                    const Data::GetterWrapper& getterWrapper = getterWrapperPair.second;
                    Data::PropertyMetadata propertyAccount;
                    propertyAccount.m_propertyType = getterWrapper.m_propertyType;
                    propertyAccount.m_propertyName = propertyName;
                    
                    const AZStd::string resultSlotName(AZStd::string::format("%s: %s", propertyName.data(), Data::GetName(getterWrapper.m_propertyType)));
                    propertyAccount.m_propertySlotId = AddOutputTypeSlot(resultSlotName, "", getterWrapper.m_propertyType, OutputStorage::Optional);
                    
                    propertyAccount.m_getterFunction = getterWrapper.m_getterFunction;
                    m_propertyAccounts.push_back(propertyAccount);
                }
            }

            void ExtractProperty::ClearPropertySlots()
            {
                for (auto&& propertyAccount : m_propertyAccounts)
                {
                    RemoveSlot(propertyAccount.m_propertySlotId);
                }
                m_propertyAccounts.clear();
            }

            AZStd::vector<AZStd::pair<AZStd::string_view, SlotId>> ExtractProperty::GetPropertyFields() const
            {
                AZStd::vector<AZStd::pair<AZStd::string_view, SlotId>> propertyFields;
                for (auto&& propertyAccount : m_propertyAccounts)
                {
                    propertyFields.emplace_back(propertyAccount.m_propertyName, propertyAccount.m_propertySlotId);
                }

                return propertyFields;
            }

            void ExtractProperty::RefreshGetterFunctions()
            {
                Data::Type sourceType = GetSourceSlotDataType();
                if (!sourceType.IsValid())
                {
                    return;
                }

                auto getterWrapperMap = Data::ExplodeToGetters(sourceType);

                for (auto&& propertyAccount : m_propertyAccounts)
                {
                    if (!propertyAccount.m_getterFunction)
                    {
                        auto foundPropIt = getterWrapperMap.find(propertyAccount.m_propertyName);
                        if (foundPropIt != getterWrapperMap.end() && propertyAccount.m_propertyType.IS_A(foundPropIt->second.m_propertyType))
                        {
                            propertyAccount.m_getterFunction = foundPropIt->second.m_getterFunction;
                        }
                        else
                        {
                            AZ_Error("Script Canvas", false, "Property (%s : %s) getter method could not be found in Data::PropertyTraits or the property type has changed."
                                " Output will not be pushed on the property's slot.",
                                propertyAccount.m_propertyName, Data::GetName(propertyAccount.m_propertyType));
                        }
                    }
                }
            }
        }
    }
}

#include <Include/ScriptCanvas/Libraries/Core/ExtractProperty.generated.cpp>
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

#include "precompiled.h"

#include <ScriptCanvas/Variable/VariableDatum.h>
#include <ScriptCanvas/Variable/VariableBus.h>

namespace ScriptCanvas
{
    bool VariableDatumVersionConverter(AZ::SerializeContext& context, AZ::SerializeContext::DataElementNode& rootElementNode)
    {
        if (rootElementNode.GetVersion() == 0)
        {
            AZ::Uuid variableId;
            if (!rootElementNode.GetChildData(AZ_CRC("m_variableId", 0x872cb185), variableId))
            {
                AZ_Error("Script Canvas", false, "Variable id in version 0 VariableDatum element should be AZ::Uuid");
                return false;
            }

            rootElementNode.RemoveElementByName(AZ_CRC("m_variableId", 0x872cb185));
            rootElementNode.AddElementWithData(context, "m_variableId", VariableId(variableId));
        }

        if (rootElementNode.GetVersion() <= 1)
        {
            const AZ::Crc32 k_input = AZ_CRC("ExposeInput", 0xcd41aead);

            AZ::Crc32 exposeType;
            if (rootElementNode.GetChildData(AZ_CRC("m_exposeCrc", 0xf5fa57bd), exposeType))
            {
                rootElementNode.RemoveElementByName(AZ_CRC("m_exposeCrc", 0xf5fa57bd));

                bool addAsInput = (exposeType == k_input);
                rootElementNode.AddElementWithData(context, "m_exposeAsInput", addAsInput);
            }
        }

        if (rootElementNode.GetVersion() <= 2)
        {
            VariableId variableId;
            if (!rootElementNode.GetChildData(AZ_CRC("m_variableId", 0x872cb185), variableId))
            {
                AZ_Error("Script Canvas", false, "Unable to find variable id on Variable Datum version %u. Conversion failed.", rootElementNode.GetVersion());
                return false;
            }
            rootElementNode.RemoveElementByName(AZ_CRC("m_variableId", 0x872cb185));

            Datum datumValue;
            if (!rootElementNode.GetChildData(AZ_CRC("m_data", 0x335cc942), datumValue))
            {
                AZ_Error("Script Canvas", false, "Unable to find datum value on Variable Datum version %u. Conversion failed.", rootElementNode.GetVersion());
                return false;
            }

            rootElementNode.RemoveElementByName(AZ_CRC("m_data", 0x335cc942));

            VariableDatum preConvertedVarDatum;
            if (!rootElementNode.GetData(preConvertedVarDatum))
            {
                AZ_Error("Script Canvas", false, "Unable to retrieved unconverted Variable Datum for version %u. Conversion failed.", rootElementNode.GetVersion());
                return false;
            }

            preConvertedVarDatum.m_id = variableId;
            preConvertedVarDatum.m_data = datumValue;

            if (!rootElementNode.SetData(context, preConvertedVarDatum))
            {
                AZ_Error("Script Canvas", false, "Unable to set converted Variable Datum. Conversion failed.");
                return false;
            }
        }

        return true;
    }

    void VariableDatum::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<VariableDatum, VariableDatumBase>()
                ->Version(4, &VariableDatumVersionConverter)
                ->Field("m_exposeAsInput", &VariableDatum::m_exposeAsInput)
                ->Field("m_inputControlVisibility", &VariableDatum::m_inputControlVisibility)
                ;

            if (auto editContext = serializeContext->GetEditContext())
            {
                editContext->Class<VariableDatum>("Variable", "Represents a Variable field within a Script Canvas Graph")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Visibility, &VariableDatum::GetVisibility)
                    ->DataElement(AZ::Edit::UIHandlers::CheckBox, &VariableDatum::m_exposeAsInput, "Expose On Component", "Controls whether or not this value is configurable from a Script Canvas Component")
                        ->Attribute(AZ::Edit::Attributes::Visibility, &VariableDatum::GetInputControlVisibility)
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &VariableDatum::OnExposureChanged);
                ;
            }
        }
    }

    VariableDatum::VariableDatum()
        : m_exposeAsInput(false)
        , m_inputControlVisibility(AZ::Edit::PropertyVisibility::Show)
        , m_visibility(AZ::Edit::PropertyVisibility::ShowChildrenOnly)
    {
    }

    VariableDatum::VariableDatum(const Datum& datum)
        : VariableDatumBase(datum)
        , m_exposeAsInput(false)
        , m_inputControlVisibility(AZ::Edit::PropertyVisibility::Show)
        , m_visibility(AZ::Edit::PropertyVisibility::ShowChildrenOnly)
    {
    }

    VariableDatum::VariableDatum(Datum&& datum)
        : VariableDatumBase(AZStd::move(datum))
        , m_exposeAsInput(false)
        , m_inputControlVisibility(AZ::Edit::PropertyVisibility::Show)
        , m_visibility(AZ::Edit::PropertyVisibility::ShowChildrenOnly)
    {
    }

    void VariableDatum::OnExposureChanged()
    {
        VariableNotificationBus::Event(m_id, &VariableNotifications::OnVariableExposureChanged);
    }

    bool VariableDatum::operator==(const VariableDatum& rhs) const
    {
        if (this == &rhs)
        {
            return true;
        }

        return static_cast<const VariableDatumBase&>(*this) == static_cast<const VariableDatumBase&>(rhs);
    }

    bool VariableDatum::operator!=(const VariableDatum& rhs) const
    {
        return !operator==(rhs);
    }

    AZ::Crc32 VariableDatum::GetInputControlVisibility() const
    {
        return m_inputControlVisibility;
    }

    void VariableDatum::SetInputControlVisibility(const AZ::Crc32& inputControlVisibility)
    {
        m_inputControlVisibility = inputControlVisibility;
    }

    AZ::Crc32 VariableDatum::GetVisibility() const
    {
        return m_visibility;
    }

    void VariableDatum::SetVisibility(AZ::Crc32 visibility)
    {
        m_visibility = visibility;
    }

    void VariableNameValuePair::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<VariableNameValuePair>()
                ->Version(0)
                ->Field("m_name", &VariableNameValuePair::m_varName)
                ->Field("m_value", &VariableNameValuePair::m_varDatum)
                ;
            if (auto editContext = serializeContext->GetEditContext())
            {
                editContext->Class<VariableNameValuePair>("Variable Element", "Represents a mapping of name to value")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
                        ->Attribute(AZ::Edit::Attributes::ChildNameLabelOverride, &VariableNameValuePair::m_varName)
                        ->Attribute(AZ::Edit::Attributes::NameLabelOverride, &VariableNameValuePair::m_varName)
                        ->Attribute(AZ::Edit::Attributes::DescriptionTextOverride, &VariableNameValuePair::GetDescriptionOverride)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &VariableNameValuePair::m_varDatum, "value", "Variable value")
                        ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
                    ;
            }
        }
    }

    AZStd::string VariableNameValuePair::GetDescriptionOverride()
    {
        return Data::GetName(m_varDatum.GetData().GetType());
    }

    void VariableDatum::GenerateNewId()
    {
        m_id = VariableId::MakeVariableId();
    }
}
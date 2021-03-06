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

#pragma once

#include <AzCore/Memory/SystemAllocator.h>
#include <SceneAPI/SceneCore/DataTypes/Rules/IRule.h>
#include <SceneAPI/SceneData/SceneDataConfiguration.h>


namespace AZ
{
    class ReflectContext;
    namespace SceneAPI
    {
        namespace Containers
        {
            class Scene;
        }

        namespace DataTypes
        {
            class IGroup;
        }
    }
}

namespace EMotionFX
{
    namespace Pipeline
    {
        namespace Rule
        {
            class MetaDataRule
                : public AZ::SceneAPI::DataTypes::IRule
            {
            public:
                AZ_RTTI(MetaDataRule, "{8D759063-7D2E-4543-8EB3-AB510A5886CF}", AZ::SceneAPI::DataTypes::IRule);
                AZ_CLASS_ALLOCATOR(MetaDataRule, AZ::SystemAllocator, 0)

                MetaDataRule() = default;
                MetaDataRule(const char* metaData);

                /**
                 * Get the string containing the list of commands representing the changes the user did on the source asset.
                 * @result A string called meta data containing a list of commands.
                 */
                const AZStd::string& GetMetaData() const;

                /**
                 * Set the meta data string which contains a list of commands representing the changes the user did on the source asset.
                 * This string can be constructed using CommandSystem::GenerateMotionMetaData() and CommandSystem::GenerateActorMetaData().
                 * @param metaData The meta data string containing a list of commands to be applied on the source asset.
                 */
                void SetMetaData(const char* metaData);

                /**
                 * Get the meta data string from the group. Search the rule container of the given group for a meta data rule and read out the meta data string.
                 * @param group The group to search for the meta data.
                 * @param[out] outMetaDataString Holds the meta data string after the operation has been completed. String will be empty in case something failed or there is no meta data.
                 */
                static bool LoadMetaData(const AZ::SceneAPI::DataTypes::IGroup& group, AZStd::string& outMetaDataString);

                /**
                 * Set the meta data string to the given group. Search the rule container of the given group for a meta data rule, create one in case there is none yet
                 * and set the given meta data string to the rule.
                 * @param group The group to set the meta data for.
                 * @param metaDataString The meta data string to set. In case the string is empty, any existing meta data rule will be removed.
                 */
                static void SaveMetaData(AZ::SceneAPI::Containers::Scene& scene, AZ::SceneAPI::DataTypes::IGroup& group, const AZStd::string& metaDataString);

                template<typename T>
                static bool SaveMetaDataToFile(const AZStd::string& sourceAssetFilename, const AZStd::string& groupName, const AZStd::string& metaDataString);

                static void Reflect(AZ::ReflectContext* context);

            protected:
                AZStd::string m_metaData;
            };

        } // Rule
    } // Pipeline
} // EMotionFX
#include <SceneAPIExt/Rules/MetaDataRule.inl>
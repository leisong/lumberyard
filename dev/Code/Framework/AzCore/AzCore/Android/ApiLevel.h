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

#include <android/api-level.h>

#include <AzCore/Android/Utils.h>


// the following defines provide cross compatibily between NDK and header versions as they
// were only officially added to the unified headers in NDK r14
#ifndef __ANDROID_API_K__
    #define __ANDROID_API_K__ 19
#endif

#ifndef __ANDROID_API_L__
    #define __ANDROID_API_L__ 21
#endif

#ifndef __ANDROID_API_L_MR1__
    #define __ANDROID_API_L_MR1__ 22
#endif

#ifndef __ANDROID_API_M__
    #define __ANDROID_API_M__ 23
#endif

#ifndef __ANDROID_API_N__
    #define __ANDROID_API_N__ 24
#endif

#ifndef __ANDROID_API_N_MR1__
    #define __ANDROID_API_N_MR1__ 25
#endif

#ifndef __ANDROID_API_O__
    #define __ANDROID_API_O__ 26
#endif

#ifndef __ANDROID_API_O_MR1__
    #define __ANDROID_API_O_MR1__ 27
#endif


namespace AZ
{
    namespace Android
    {
        //! Supported API level codes for runtime checks
        enum class ApiLevel : unsigned char
        {
            KitKat          = __ANDROID_API_K__,
            Lollipop        = __ANDROID_API_L__,
            Lollipop_mr1    = __ANDROID_API_L_MR1__,
            Marshmallow     = __ANDROID_API_M__,
            Nougat          = __ANDROID_API_N__,
            Nougat_mr1      = __ANDROID_API_N_MR1__,
            Oreo            = __ANDROID_API_O__,
            Oreo_mr1        = __ANDROID_API_O_MR1__,
        };

        //! Request the OS runtime API level of the device
        AZ_INLINE ApiLevel GetRuntimeApiLevel()
        {
            AConfiguration* config = Utils::GetConfiguration();
            ApiLevel sdkVersion = static_cast<ApiLevel>(AConfiguration_getSdkVersion(config));
            AZ_Assert(sdkVersion >= ApiLevel::KitKat, "The Android runtime API level detected (%d) is unsupported", sdkVersion);
            return sdkVersion;
        }
    }
}

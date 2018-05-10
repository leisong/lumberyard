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

#include <AzCore/Math/Aabb.h>
#include <AzCore/Math/Obb.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Math/Color.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Math/Matrix3x3.h>
#include <AzCore/Math/Quaternion.h>
#include <AzCore/Component/EntityId.h>
#include <AzCore/Math/Plane.h>
#include <Cry_Math.h>
#include <Cry_Geo.h>
#include <Cry_Color.h>

inline AZ::Vector3 LYVec3ToAZVec3(const Vec3& source)
{
    return AZ::Vector3(source.x, source.y, source.z);
}

inline AZ::Vector4 LYVec4ToAZVec4(const Vec4& source)
{
    return AZ::Vector4(source.x, source.y, source.z, source.w);
}

inline Vec3 AZVec3ToLYVec3(const AZ::Vector3& source)
{
    return Vec3(source.GetX(), source.GetY(), source.GetZ());
}

inline Vec4 AZVec4ToLYVec4(const AZ::Vector4& source)
{
    return Vec4(source.GetX(), source.GetY(), source.GetZ(), source.GetW());
}

inline AZ::Color LYVec3ToAZColor(const Vec3& source)
{
    return AZ::Color(source.x, source.y, source.z, 1.0f);
}

inline Vec3 AZColorToLYVec3(const AZ::Color& source)
{
    return Vec3(source.GetR(), source.GetG(), source.GetB());
}

inline Vec4 AZColorToLYVec4(const AZ::Color& source)
{
    return Vec4(source.GetR(), source.GetG(), source.GetB(), source.GetA());
}

inline ColorF AZColorToLYColorF(const AZ::Color& source)
{
    return ColorF(source.ToU32());
}

inline AZ::Color LYColorFToAZColor(const ColorF& source)
{
    return AZ::Color(source.r, source.g, source.b, source.a);
}

inline AZ::Quaternion LYQuaternionToAZQuaternion(const Quat& source)
{
    const float f4[4] = { source.v.x, source.v.y, source.v.z, source.w };
    return AZ::Quaternion::CreateFromFloat4(f4);
}

inline Quat AZQuaternionToLYQuaternion(const AZ::Quaternion& source)
{
    float f4[4];
    source.StoreToFloat4(f4);
    return Quat(f4[3], f4[0], f4[1], f4[2]);
}

inline Matrix34 AZTransformToLYTransform(const AZ::Transform& source)
{
    return Matrix34::CreateFromVectors(
        AZVec3ToLYVec3(source.GetColumn(0)),
        AZVec3ToLYVec3(source.GetColumn(1)),
        AZVec3ToLYVec3(source.GetColumn(2)),
        AZVec3ToLYVec3(source.GetTranslation()));
}

inline Matrix33 AZMatrix3x3ToLYMatrix3x3(const AZ::Matrix3x3& source)
{
    return Matrix33::CreateFromVectors(
        AZVec3ToLYVec3(source.GetColumn(0)),
        AZVec3ToLYVec3(source.GetColumn(1)),
        AZVec3ToLYVec3(source.GetColumn(2)));
}

inline AZ::Matrix3x3 LyMatrix3x3ToAzMatrix3x3(const Matrix33& source)
{
    return AZ::Matrix3x3::CreateFromColumns(
        LYVec3ToAZVec3(source.GetColumn(0)),
        LYVec3ToAZVec3(source.GetColumn(1)),
        LYVec3ToAZVec3(source.GetColumn(2)));
}

inline AZ::Transform LYTransformToAZTransform(const Matrix34& source)
{
    return AZ::Transform::CreateFromRowMajorFloat12(source.GetData());
}

inline AZ::Transform LYQuatTToAZTransform(const QuatT& source)
{
    return AZ::Transform::CreateFromQuaternionAndTranslation(
        LYQuaternionToAZQuaternion(source.q),
        LYVec3ToAZVec3(source.t));
}

inline QuatT AZTransformToLYQuatT(const AZ::Transform& source)
{
    AZ::Transform sourceNoScale(source);
    sourceNoScale.ExtractScale();

    return QuatT(
        AZQuaternionToLYQuaternion(AZ::Quaternion::CreateFromTransform(sourceNoScale)),
        AZVec3ToLYVec3(source.GetTranslation()));
}

inline AABB AZAabbToLyAABB(const AZ::Aabb& source)
{
    return AABB(AZVec3ToLYVec3(source.GetMin()), AZVec3ToLYVec3(source.GetMax()));
}

inline AZ::Aabb LyAABBToAZAabb(const AABB& source)
{
    return AZ::Aabb::CreateFromMinMax(LYVec3ToAZVec3(source.min), LYVec3ToAZVec3(source.max));
}

inline AZ::Obb LyOBBtoAZObb(const OBB& source)
{
    return AZ::Obb::CreateFromPositionAndAxes(
        LYVec3ToAZVec3(source.c),
        LYVec3ToAZVec3(source.m33.GetColumn0()),
        source.h.x,
        LYVec3ToAZVec3(source.m33.GetColumn1()),
        source.h.y,
        LYVec3ToAZVec3(source.m33.GetColumn2()),
        source.h.z);
}

inline OBB AZObbToLyOBB(const AZ::Obb& source)
{
    return OBB::CreateOBB(
        Matrix33::CreateFromVectors(
            AZVec3ToLYVec3(source.GetAxisX()),
            AZVec3ToLYVec3(source.GetAxisY()),
            AZVec3ToLYVec3(source.GetAxisZ())),
        Vec3(source.GetHalfLengthX(), source.GetHalfLengthY(), source.GetHalfLengthZ()),
        AZVec3ToLYVec3(source.GetPosition()));
}

inline AZ::Plane LyPlaneToAZPlane(const Plane& source)
{
    return AZ::Plane::CreateFromNormalAndDistance(LYVec3ToAZVec3(source.n), source.d);
}

inline Plane AZPlaneToLyPlane(const AZ::Plane& source)
{
    Plane resultPlane;
    resultPlane.Set(AZVec3ToLYVec3(source.GetNormal()), source.GetDistance());

    return resultPlane;
}

// returns true if an entityId is a legacyId - 0 is considered not to be a valid legacy Id as it's the
// default (but invalid) value in the legacy system
inline bool IsLegacyEntityId(const AZ::EntityId& entityId)
{
    return (((static_cast<AZ::u64>(entityId) >> 32) == 0) && static_cast<AZ::u64>(entityId) != 0);
}

inline unsigned GetLegacyEntityId(const AZ::EntityId& entityId)
{
    return IsLegacyEntityId(entityId) ? static_cast<unsigned>(static_cast<AZ::u64>(entityId)) : 0;
}

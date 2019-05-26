#pragma once
#include <Math/Type/Math/DVector3.h>
#include <Math/Common/TGlobalTypes.h>

using namespace dy::math;

class DRay final
{
public:
  DRay() {};
  DRay(const DVector3<TReal>& origin, const DVector3<TReal>& direction)
    : mOrigin{origin},
      mDirection{direction.Normalize()}
  { }

  /// @brief
  const DVector3<TReal>& GetOrigin() const noexcept { return this->mOrigin; }

  /// @brief
  const DVector3<TReal>& GetDirection() const noexcept { return this->mDirection; }

  /// @brief
  DVector3<TReal> GetPointAtParam(TReal t) const
  {
    return this->mOrigin + t * mDirection;
  }

private:
  DVector3<TReal> mOrigin;
  DVector3<TReal> mDirection;
};

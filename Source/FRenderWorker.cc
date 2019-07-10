///
/// MIT License
/// Copyright (c) 2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <FRenderWorker.hpp>
#include <Manager/MScene.hpp>
#include <XCommon.hpp>
#include <Object/FCamera.hpp>

namespace ray
{

void FRenderWorker::Execute(
  const FCamera& cam,
  const std::vector<DUVec2>& list, 
  const DUVec2 imgSize, 
  DDynamicGrid2D<DVec3>& container)
{
  for (const auto& index : list)
  {
    const auto rayList  = cam.CreateRay(index.X, index.Y - 1);
    const auto repeat   = cam.GetRepeat();

    DVec3 colorSum = {0};
    for (TU32 r = 0; r < repeat; ++r)
    {
      for (const auto& ray : rayList) 
      { 
        colorSum += EXPR_SGT(MScene).ProceedRay(ray, 0, 32);
      }
    }
    colorSum /= (TReal(rayList.size()) * repeat);
    container.Set(index.X, imgSize.Y - index.Y, colorSum);
  }

  // If there is -nan, change value into 0. (I suggest this is a bug.)
  for (TIndex y = 0, ySize = imgSize.Y; y < ySize; ++y)
  {
    for (TIndex x = 0, xSize = imgSize.X; x < xSize; ++x)
    {
      auto& color = container.Get(x, y);
      for (TIndex i = 0; i < 3; ++i)
      {
        if (std::isnan(color[i]) == true) { color[i] = 0; }
      }
    }
  }
}

} /// ::ray namespace
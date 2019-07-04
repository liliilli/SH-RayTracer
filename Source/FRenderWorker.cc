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
  DDynamicGrid2D<DIVec3>& container)
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

    // Encoding
    auto encode = 1.0f / cam.GetGamma();
    for (int i = 0; i < 3; ++i) { colorSum[i] = std::pow(colorSum[i], encode); }

    // Clamping 
    for (int i = 0; i < 3; ++i) { colorSum[i] = std::clamp(colorSum[i], TReal(0), TReal(1)); }

    int ir = int(255.99f * colorSum[0]);
    int ig = int(255.99f * colorSum[1]);
    int ib = int(255.99f * colorSum[2]);
    container.Set(index.X, imgSize.Y - index.Y, {ir, ig, ib});
  }
}

} /// ::ray namespace
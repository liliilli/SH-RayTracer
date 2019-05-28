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
#include <MScene.hpp>

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
    auto rayList = cam.CreateRay(index.X, index.Y - 1);

    DVec3 colorSum = {0};
    for (const auto& ray : rayList) 
    { 
      colorSum += EXPR_SGT(MScene).ProceedRay(ray);
    }
    colorSum /= TReal(rayList.size());

    int ir = int(255.99f * colorSum[0]);
    int ig = int(255.99f * colorSum[1]);
    int ib = int(255.99f * colorSum[2]);
    container.Set(index.X, imgSize.Y - index.Y, {ir, ig, ib});
  }
}

} /// ::ray namespace
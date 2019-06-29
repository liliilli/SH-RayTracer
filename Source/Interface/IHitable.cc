///
/// MIT License
/// Copyright (c) 2018-2019 Jongmin Yun
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///

#include <Interface/IHitable.hpp>

namespace ray
{

IHitable::IHitable(EShapeType type, const IMaterial*& pMaterial)
  : IObject { EObject::Hitable },
    mMaterialType { type },
    mpMaterial{ pMaterial }
{ }

EShapeType IHitable::GetType() const noexcept 
{ 
  return this->mMaterialType; 
}

const IMaterial* IHitable::GetMaterial() const noexcept 
{
  return this->mpMaterial; 
}

bool IHitable::HasAABB() const noexcept 
{ 
  return this->mAABB != nullptr; 
}

const DAABB* IHitable::GetAABB() const noexcept 
{ 
  return this->mAABB.get(); 
}

} /// ::ray namespace
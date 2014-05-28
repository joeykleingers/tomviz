/******************************************************************************

  This source file is part of the TEM tomography project.

  Copyright Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/
#ifndef __TEM_ModuleOrthogonalSlice_h
#define __TEM_ModuleOrthogonalSlice_h

#include "Module.h"
#include "vtkWeakPointer.h"

class vtkSMProxy;

namespace TEM
{
class ModuleOrthogonalSlice : public Module
{
  Q_OBJECT;
  typedef Module Superclass;
public:
  ModuleOrthogonalSlice(QObject* parent=NULL);
  virtual ~ModuleOrthogonalSlice();

  virtual QString label() const { return  "Orthogonal Slice"; }
  virtual QIcon icon() const;
  virtual bool initialize(vtkSMSourceProxy* dataSource, vtkSMViewProxy* view);
  virtual bool finalize();
  virtual bool setVisibility(bool val);
  virtual bool visibility() const;
  virtual void addToPanel(pqProxiesWidget*);

private:
  Q_DISABLE_COPY(ModuleOrthogonalSlice);
  vtkWeakPointer<vtkSMSourceProxy> PassThrough;
  vtkWeakPointer<vtkSMProxy> Representation;
};

}
#endif

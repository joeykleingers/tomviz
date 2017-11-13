/******************************************************************************

  This source file is part of the tomviz project.

  Copyright Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/
#ifndef tomvizsimplformat_h
#define tomvizsimplformat_h

#include <QtCore/QString>

#include "hdf5.h"

class vtkImageData;
class DataContainerArrayProxy;

namespace tomviz {

class DataSource;

class SIMPLFormat
{
public:
  SIMPLFormat();
  ~SIMPLFormat();

  bool read(const QString &fileName, vtkImageData* data);

private:

  bool readData(hid_t locId, const QString &path, vtkImageData* data);

};
}

#endif // tomvizsimplformat_h

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
#include "SIMPLFormat.h"

#include "DataSource.h"

#include <vtkDataArray.h>
#include <vtkImageData.h>

#include <vtkSMSourceProxy.h>

#include "vtk_hdf5.h"

#include "SIMPLDataSelectionDialog.h"

#include <cassert>
#include <string>
#include <vector>

#include <iostream>

#include "H5Support/HDF5ScopedFileSentinel.h"

#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/ImageGeom.h"

namespace tomviz {

SIMPLFormat::SIMPLFormat()
{
}

bool SIMPLFormat::read(const QString &fileName, vtkImageData* image)
{
  SIMPLH5DataReader::Pointer simplReader = SIMPLH5DataReader::New();
  simplReader->openFile(fileName);

  int err = 0;
  SIMPLH5DataReaderRequirements req(SIMPL::Defaults::AnyPrimitive, 1, AttributeMatrix::Type::Any, IGeometry::Type::Image);
  DataContainerArrayProxy proxy = simplReader->readDataContainerArrayStructure(req, err);
  if (err < 0)
  {
    return false;
  }

  QStringList pathStrs = proxy.flattenHeirarchy();
  QList<DataArrayPath> dataArrayPaths;
  for (int i = 0; i < pathStrs.size(); i++)
  {
    QString pathStr = pathStrs[i];
    DataArrayPath path = DataArrayPath::Deserialize(pathStr, "|");
    dataArrayPaths.push_back(path);
  }

  SIMPLDataSelectionDialog::Pointer dialog = SIMPLDataSelectionDialog::New();
  dialog->setDataArrayPaths(dataArrayPaths);
  int result = dialog->exec();
  if (result == QDialog::Accepted)
  {
    QString daPath = dialog->getSelectedDataArrayPath();

    hid_t fileId = H5Fopen(fileName.toStdString().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (fileId < 0) {
      return false;
    }

    hid_t dcGrpId = H5Gopen(fileId, SIMPL::StringConstants::DataContainerGroupName.toStdString().c_str(), H5P_DEFAULT);
    if (dcGrpId < 0) {
      H5Fclose(fileId);
      return false;
    }

    bool readResult = readData(dcGrpId, daPath, image);

    H5Gclose(dcGrpId);
    H5Fclose(fileId);
    return readResult;
  }

  return false;
}

bool SIMPLFormat::readData(hid_t locId, const QString &path, vtkImageData* data)
{
  std::vector<int> dims;

  hid_t datasetId = H5Dopen(locId, path.toStdString().c_str(), H5P_DEFAULT);
  if (datasetId < 0) {
    return false;
  }

  hid_t dataspaceId = H5Dget_space(datasetId);
  if (dataspaceId < 0) {
    H5Dclose(datasetId);
    return false;
  }

  int dimCount = H5Sget_simple_extent_ndims(dataspaceId);
  if (dimCount < 1) {
    H5Dclose(datasetId);
    return false;
  }

  hsize_t* h5dims = new hsize_t[dimCount];
  int dimCount2 = H5Sget_simple_extent_dims(dataspaceId, h5dims, nullptr);
  if (dimCount == dimCount2) {
    dims.resize(dimCount);
    std::copy(h5dims, h5dims + dimCount, dims.begin());
  }
  if (dimCount == 3) {
    dims[0] = h5dims[2];
    dims[2] = h5dims[0];
  }
  delete[] h5dims;

  // Map the HDF5 types to the VTK types for storage and memory. We should
  // probably add more, but I got the important ones for testing in first.
  int vtkDataType = VTK_FLOAT;
  hid_t dataTypeId = H5Dget_type(datasetId);
  hid_t memTypeId = 0;

  if (H5Tequal(dataTypeId, H5T_IEEE_F64LE)) {
    memTypeId = H5T_NATIVE_DOUBLE;
    vtkDataType = VTK_DOUBLE;
  } else if (H5Tequal(dataTypeId, H5T_IEEE_F32LE)) {
    memTypeId = H5T_NATIVE_FLOAT;
    vtkDataType = VTK_FLOAT;
  } else if (H5Tequal(dataTypeId, H5T_STD_I32LE)) {
    memTypeId = H5T_NATIVE_INT;
    vtkDataType = VTK_INT;
  } else if (H5Tequal(dataTypeId, H5T_STD_U32LE)) {
    memTypeId = H5T_NATIVE_UINT;
    vtkDataType = VTK_UNSIGNED_INT;
  } else if (H5Tequal(dataTypeId, H5T_STD_I16LE)) {
    memTypeId = H5T_NATIVE_SHORT;
    vtkDataType = VTK_SHORT;
  } else if (H5Tequal(dataTypeId, H5T_STD_U16LE)) {
    memTypeId = H5T_NATIVE_USHORT;
    vtkDataType = VTK_UNSIGNED_SHORT;
  } else if (H5Tequal(dataTypeId, H5T_STD_I8LE)) {
      memTypeId = H5T_NATIVE_CHAR;
      vtkDataType = VTK_SIGNED_CHAR;
  } else if (H5Tequal(dataTypeId, H5T_STD_U8LE)) {
    memTypeId = H5T_NATIVE_UCHAR;
    vtkDataType = VTK_UNSIGNED_CHAR;
  } else {
    // Not accounted for, fail for now, should probably improve this soon.
    std::cout << "Unknown type encountered!" << dataTypeId << std::endl;
    H5Dclose(datasetId);
    return false;
  }

  data->SetDimensions(&dims[0]);
  data->AllocateScalars(vtkDataType, 1);

  H5Dread(datasetId, memTypeId, H5S_ALL, dataspaceId, H5P_DEFAULT,
          data->GetScalarPointer());
  data->Modified();

  H5Dclose(datasetId);
  return true;
}

SIMPLFormat::~SIMPLFormat()
{

}
}

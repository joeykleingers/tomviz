/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "SIMPLDataSelectionDialog.h"

#include "SIMPLDataListWidgetModel.h"
#include "SIMPLDataListWidgetItemDelegate.h"

#include "ui_SIMPLDataSelectionDialog.h"

namespace tomviz {

SIMPLDataSelectionDialog::SIMPLDataSelectionDialog(QWidget* parent) :
  QDialog(parent),
  m_ui(new Ui::SIMPLDataSelectionDialog)
{
  m_ui->setupUi(this);

  setupGui();
}

SIMPLDataSelectionDialog::~SIMPLDataSelectionDialog()
{

}

void SIMPLDataSelectionDialog::setupGui()
{
  SIMPLDataListWidgetItemDelegate* delegate = new SIMPLDataListWidgetItemDelegate(m_ui->listView);
  m_ui->listView->setItemDelegate(delegate);

//  m_ui->importBtn->setEnabled(false);

  createConnections();
}

void SIMPLDataSelectionDialog::setDataArrayPaths(QList<DataArrayPath> daPaths)
{
  QAbstractItemModel* oldModel = m_ui->listView->model();
  if (oldModel != nullptr)
  {
    delete oldModel;
  }

  SIMPLDataListWidgetModel* model = new SIMPLDataListWidgetModel();
  model->insertColumn(0);
  for (int i = 0; i < daPaths.size(); i++)
  {
    DataArrayPath daPath = daPaths[i];
    QString path = daPath.serialize("/");

    int row = model->rowCount();
    model->insertRow(row);
    model->setData(model->index(row, 0), path, Qt::DisplayRole);
  }

  m_ui->listView->setModel(model);
}

void SIMPLDataSelectionDialog::createConnections()
{
  connect(m_ui->importBtn, &QPushButton::clicked, [=] { accept(); });
  connect(m_ui->cancelBtn, &QPushButton::clicked, [=] { reject(); });

//  connect(m_ui->listView->selectionModel(), &QItemSelectionModel::currentChanged, this, [=] (const QModelIndex &cur, const QModelIndex &prev) {
//    Q_UNUSED(prev)

//    m_ui->importBtn->setEnabled(cur.isValid());
//  });
}

QString SIMPLDataSelectionDialog::getSelectedDataArrayPath()
{
  QModelIndex index = m_ui->listView->currentIndex();
  SIMPLDataListWidgetModel* model = dynamic_cast<SIMPLDataListWidgetModel*>(m_ui->listView->model());
  if (model == nullptr)
  {
    return QString();
  }

  QString daPath = model->getArrayPath(index);
  return daPath;
}

}

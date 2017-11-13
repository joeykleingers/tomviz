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

#include "SIMPLDataListWidgetModel.h"

#include "SIMPLib/DataContainers/DataArrayPath.h"

#include <QtWidgets>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataListWidgetModel::SIMPLDataListWidgetModel(QObject* parent)
: QAbstractListModel(parent)
{
  m_RootItem = new SIMPLDataListWidgetItem("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataListWidgetModel::~SIMPLDataListWidgetModel()
{
  delete m_RootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataListWidgetItem* SIMPLDataListWidgetModel::insertItem(const int row, const QString &arrayPath, SIMPLDataListWidgetItem *headerItem)
{
  int rowPos;
  if (headerItem)
  {
    rowPos = getIndex(headerItem).row() + row + 1;
  }
  else
  {
    rowPos = row;
  }

  insertRow(rowPos, QModelIndex());

  QModelIndex newNameIndex = index(rowPos, SIMPLDataListWidgetItem::DefaultColumn, QModelIndex());
  SIMPLDataListWidgetItem* newItem = getItem(newNameIndex);
  newItem->setArrayPath(arrayPath);

  return newItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLDataListWidgetModel::removeItem(const int row, SIMPLDataListWidgetItem* headerItem)
{
  int rowPos;
  if (headerItem)
  {
    rowPos = getIndex(headerItem).row() + row + 1;
  }
  else
  {
    rowPos = row;
  }

  removeRow(rowPos);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant SIMPLDataListWidgetModel::data(const QModelIndex& index, int role) const
{
  if(!index.isValid())
  {
    return QVariant();
  }

  SIMPLDataListWidgetItem* item = getItem(index);

  if(role == Qt::DisplayRole)
  {
    QString arrayPath = item->getArrayPath();
    DataArrayPath daPath = DataArrayPath::Deserialize(arrayPath, "/");
    QString displayStr = "<b>" + daPath.getDataArrayName() + "</b>";
    displayStr.append("<br>" + arrayPath);

    return displayStr;
  }
  else if(role == Qt::ForegroundRole)
  {
    return QColor(Qt::black);
  }
  else if(role == Qt::ToolTipRole)
  {
    return item->getItemTooltip();
  }
  else if(role == Qt::DecorationRole)
  {
    QModelIndex nameIndex = this->index(index.row(), SIMPLDataListWidgetItem::DefaultColumn, index.parent());
    if(nameIndex == index)
    {
      return item->getIcon();
    }
    else
    {
      return QVariant();
    }
  }
  else
  {
    return QVariant();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SIMPLDataListWidgetModel::getArrayPath(const QModelIndex& index)
{
  SIMPLDataListWidgetItem* item = getItem(index);
  return item->getArrayPath();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags SIMPLDataListWidgetModel::flags(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return 0;
  }

  return (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataListWidgetItem* SIMPLDataListWidgetModel::getItem(const QModelIndex& index) const
{
  if(index.isValid())
  {
    SIMPLDataListWidgetItem* item = static_cast<SIMPLDataListWidgetItem*>(index.internalPointer());
    if(item)
    {
      return item;
    }
  }
  return m_RootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex SIMPLDataListWidgetModel::getIndex(const SIMPLDataListWidgetItem* item) const
{
  int rowCount = this->rowCount(QModelIndex());
  for (int i = 0; i < rowCount; i++)
  {
    QModelIndex index = this->index(i, 0);
    SIMPLDataListWidgetItem* curItem = getItem(index);
    if (item == curItem)
    {
      return index;
    }
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex SIMPLDataListWidgetModel::index(int row, int column, const QModelIndex& parent) const
{
  if(parent.isValid() && parent.column() != 0)
  {
    return QModelIndex();
  }

  SIMPLDataListWidgetItem* childItem = m_RootItem->child(row);
  if(childItem)
  {
    return createIndex(row, column, childItem);
  }
  else
  {
    return QModelIndex();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLDataListWidgetModel::insertRows(int position, int rows, const QModelIndex& parent)
{
  SIMPLDataListWidgetItem* parentItem = getItem(parent);
  bool success;

  beginInsertRows(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows, SIMPLDataListWidgetItem::DefaultColumnCount);
  endInsertRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLDataListWidgetModel::removeRows(int position, int rows, const QModelIndex& parent)
{
  SIMPLDataListWidgetItem* parentItem = getItem(parent);
  bool success = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLDataListWidgetModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
  beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild);

  SIMPLDataListWidgetItem* srcParentItem = getItem(sourceParent);
  SIMPLDataListWidgetItem* destParentItem = getItem(destinationParent);

  for(int i = sourceRow; i < sourceRow + count; i++)
  {
    QModelIndex srcIndex = index(i, SIMPLDataListWidgetItem::DefaultColumn, sourceParent);
    SIMPLDataListWidgetItem* srcItem = getItem(srcIndex);

    destParentItem->insertChild(destinationChild, srcItem);
    srcItem->setParent(destParentItem);
    srcParentItem->removeChild(i);
  }

  endMoveRows();

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex SIMPLDataListWidgetModel::parent(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return QModelIndex();
  }

  SIMPLDataListWidgetItem* childItem = getItem(index);
  SIMPLDataListWidgetItem* parentItem = childItem->parent();

  if(parentItem == m_RootItem)
  {
    return QModelIndex();
  }

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SIMPLDataListWidgetModel::rowCount(const QModelIndex& parent) const
{
  SIMPLDataListWidgetItem* parentItem = getItem(parent);

  return parentItem->childCount();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLDataListWidgetModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  SIMPLDataListWidgetItem* item = getItem(index);
  bool result = false;

  if (role == Qt::DisplayRole)
  {
    result = item->setArrayPath(value.toString());
  }
  else if(role == Qt::DecorationRole)
  {
    result = item->setIcon(value.value<QIcon>());
  }
  else if(role == Qt::ToolTipRole)
  {
    result = item->setItemTooltip(value.toString());
  }

  if(result)
  {
    emit dataChanged(index, index);
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataListWidgetItem* SIMPLDataListWidgetModel::getRootItem()
{
  return m_RootItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLDataListWidgetModel::isEmpty()
{
  if(rowCount(QModelIndex()) <= 0)
  {
    return true;
  }
  return false;
}

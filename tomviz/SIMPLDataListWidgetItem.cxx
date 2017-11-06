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

#include "SIMPLDataListWidgetItem.h"

#include <QtCore/QStringList>
#include <QtGui/QColor>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataListWidgetItem::SIMPLDataListWidgetItem(const QString &name, const QString &path, SIMPLDataListWidgetItem* parent) :
  m_ItemName(name)
, m_ItemPath(path)
, m_ParentItem(parent)
, m_ItemTooltip("")
, m_Icon(QIcon())
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataListWidgetItem::~SIMPLDataListWidgetItem()
{
  qDeleteAll(m_ChildItems);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataListWidgetItem* SIMPLDataListWidgetItem::child(int number)
{
  return m_ChildItems.value(number);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SIMPLDataListWidgetItem::childCount() const
{
  return m_ChildItems.count();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SIMPLDataListWidgetItem::childNumber() const
{
  if(m_ParentItem)
  {
    return m_ParentItem->m_ChildItems.indexOf(const_cast<SIMPLDataListWidgetItem*>(this));
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLDataListWidgetItem::insertChild(int position, SIMPLDataListWidgetItem* child)
{
  m_ChildItems.insert(position, child);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLDataListWidgetItem::insertChildren(int position, int count, int columns)
{
  if(position < 0 || position > m_ChildItems.size())
  {
    return false;
  }

  for(int row = 0; row < count; ++row)
  {
    SIMPLDataListWidgetItem* item = new SIMPLDataListWidgetItem("", "", this);
    insertChild(position, item);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLDataListWidgetItem* SIMPLDataListWidgetItem::parent()
{
  return m_ParentItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLDataListWidgetItem::removeChild(int position)
{
  m_ChildItems.removeAt(position);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLDataListWidgetItem::removeChildren(int position, int count)
{
  if(position < 0 || position + count > m_ChildItems.size())
  {
    return false;
  }

  for(int row = 0; row < count; ++row)
  {
    delete m_ChildItems.takeAt(position);
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString SIMPLDataListWidgetItem::getItemTooltip()
{
  return m_ItemTooltip;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLDataListWidgetItem::setItemTooltip(const QString& value)
{
  m_ItemTooltip = value;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QIcon SIMPLDataListWidgetItem::getIcon()
{
  return m_Icon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLDataListWidgetItem::setIcon(const QIcon& icon)
{
  m_Icon = icon;
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLDataListWidgetItem::setParent(SIMPLDataListWidgetItem* parent)
{
  m_ParentItem = parent;
}

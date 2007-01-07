/**********************************************************************
  MoleculeTreeView - Molecule Widget

  Copyright (C) 2006 by Geoffrey R. Hutchison
  Some portions Copyright (C) 2006 by Donald E. Curtis

  This file is part of the Avogadro molecular editor project.  For more
  information, see <http://avogadro.sourceforge.net/>

  Some code is based on Open Babel
  For more information, see <http://openbabel.sourceforge.net/>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 ***********************************************************************/

#include "moleculetreeview.moc"

#include <QDebug>
#include <QPainter>

using namespace Avogadro;

MoleculeTreeView::MoleculeTreeView(Molecule *molecule, QWidget *parent) : QTreeWidget(parent), _molecule(NULL)
{
  setItemDelegate(new MoleculeItemDelegate(this, this));
  setRootIsDecorated(false);

  for(int t = Primitive::MoleculeType; t < Primitive::LastType; t++)
  {
    _groups.append(NULL);
  }

  addGroup("Atoms", Primitive::AtomType);
  addGroup("Bonds", Primitive::BondType);
  addGroup("Residues", Primitive::ResidueType);

  setMolecule(molecule);

  connect(this, SIGNAL(itemPressed(QTreeWidgetItem*,int)), 
      this, SLOT(handleMousePress(QTreeWidgetItem*)));
}

void MoleculeTreeView::handleMousePress(QTreeWidgetItem *item)
{
  if (item == 0)
    return;

  if(item->parent() == 0)
  {
    item->setExpanded(!item->isExpanded());
    return;
  }
}

void MoleculeTreeView::setMolecule(Molecule *molecule)
{
  if(!molecule)
    return;

  for(int t = Primitive::MoleculeType; t < Primitive::LastType; t++)
  {
    if(_groups[t])
    {
      QList<QTreeWidgetItem *> children = _groups[t]->takeChildren();
      qDeleteAll(children);
    }
  }

  _molecule = molecule;
  // add the atoms to the default queue
  std::vector<OpenBabel::OBNodeBase*>::iterator i;
  for(Atom *atom = (Atom*)_molecule->BeginAtom(i); atom; atom = (Atom*)_molecule->NextAtom(i))
  {
    addPrimitive(atom);
  }

  // add the bonds to the default queue
  std::vector<OpenBabel::OBEdgeBase*>::iterator j;
  for(Bond *bond = (Bond*)_molecule->BeginBond(j); bond; bond = (Bond*)_molecule->NextBond(j))
  {
    addPrimitive(bond);
  }

  // add the residues to the default queue
  std::vector<OpenBabel::OBResidue*>::iterator k;
  for(Residue *residue = (Residue*)_molecule->BeginResidue(k); residue;
      residue = (Residue *)_molecule->NextResidue(k)) {
    addPrimitive(residue);
  }
}

QTreeWidgetItem* MoleculeTreeView::addGroup(QString name, enum Primitive::Type type)
{
  qDebug() << "addGroup";

  QTreeWidgetItem *group = new QTreeWidgetItem(this);
  group->setText(0, name);
  group->setFlags(group->flags() & ~Qt::ItemIsSelectable);
  group->setData(0, Qt::UserRole, type);
  _groups[type] = group;

  return group;
}

QTreeWidgetItem* MoleculeTreeView::addPrimitive(Primitive *primitive)
{
  qDebug() << "addPrimitive";

  enum Primitive::Type type = primitive->type();

  QTreeWidgetItem *group = _groups[type];
  if(_groups[type] == NULL)
    return NULL;

  QTreeWidgetItem *item = new QTreeWidgetItem(group);
  QString str = "Unknown";
  if(type == Primitive::MoleculeType)
  {
    str = tr("Molecule");
  }
  else if(type == Primitive::AtomType)
  {
    Atom *atom = (Atom*)primitive;
    str = tr("Atom ") + QString::number(atom->GetIdx());
  }
  else if(type == Primitive::BondType)
  {
    Bond *bond = (Bond*)primitive;
    str = tr("Bond ") + QString::number(bond->GetIdx()) + tr(" (") + 
      QString::number(bond->GetBeginAtomIdx()) + tr(",") 
      + QString::number(bond->GetEndAtomIdx()) + tr(")");
  }
  else if(type == Primitive::ResidueType)
  {
    Residue *residue = (Residue*)primitive;
    str = tr("Residue ") + QString::number(residue->GetIdx());
  }

  item->setText(0, str);
  item->setFlags(item->flags() | Qt::ItemIsSelectable);
  item->setData(0, Qt::UserRole, primitive);

  return item;
}

  MoleculeItemDelegate::MoleculeItemDelegate(QTreeView *view, QWidget *parent)
: QItemDelegate(parent), _view(view)
{
}

void MoleculeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  const QAbstractItemModel *model = index.model();
  Q_ASSERT(model);

  if (!model->parent(index).isValid()) {
    // if we ever want to change up the style a bit.
//dc:     QStyleOptionHeader headerOption;
//dc:     headerOption.state = option.state;
//dc:     headerOption.rect = option.rect;
//dc:     headerOption.palette = option.palette;
//dc:     _view->style()->drawControl(QStyle::CE_HeaderSection, &dockOption, painter, _view);

    QPen pen = painter->pen();

    // this is a top-level item.
    QStyleOptionButton buttonOption;

    buttonOption.state = option.state;
#ifdef Q_WS_MAC
    buttonOption.state |= QStyle::State_Raised;
#endif
    buttonOption.state &= ~QStyle::State_HasFocus;

    buttonOption.rect = option.rect;
    buttonOption.palette = option.palette;
    buttonOption.features = QStyleOptionButton::None;
    _view->style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter, _view);

    QStyleOption branchOption;
    static const int i = 9; // ### hardcoded in qcommonstyle.cpp
    QRect r = option.rect;
    branchOption.rect = QRect(r.left() + i/2, r.top() + (r.height() - i)/2, i, i);
    branchOption.palette = option.palette;
    branchOption.state = QStyle::State_Children;

    if (_view->isExpanded(index))
    {
      branchOption.state |= QStyle::State_Open;
    }
    else
    {
      painter->setPen(Qt::darkGray);
    }

    _view->style()->drawPrimitive(QStyle::PE_IndicatorBranch, &branchOption, painter, _view);

    // draw text
    QRect textrect = QRect(r.left() + i*2, r.top(), r.width() - ((5*i)/2), r.height());
    QString text = elidedText(option.fontMetrics, textrect.width(), Qt::ElideMiddle, 
        model->data(index, Qt::DisplayRole).toString());
    _view->style()->drawItemText(painter, textrect, Qt::AlignCenter,
        option.palette, _view->isEnabled(), text);

    painter->setPen(pen);
  } else {
    QItemDelegate::paint(painter, option, index);
  }
}

QSize MoleculeItemDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
  QStyleOptionViewItem option = opt;
  QSize sz = QItemDelegate::sizeHint(opt, index) + QSize(2, 2);
  return sz;
}


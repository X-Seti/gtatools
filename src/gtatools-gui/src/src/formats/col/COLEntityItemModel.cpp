/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "COLEntityItemModel.h"




COLEntityItemModel::COLEntityItemModel(COLModel* model)
		: model(model)
{
}


Qt::ItemFlags COLEntityItemModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


QVariant COLEntityItemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	void* iptr = index.internalPointer();

	const COLFaceGroup* fgs = model->getFaceGroupCount() != 0 ? &model->getFaceGroups()[0] : NULL;
	const COLSphere* spheres = model->getSphereCount() != 0 ? &model->getSpheres()[0] : NULL;
	const COLBox* boxes = model->getBoxCount() != 0 ? &model->getBoxes()[0] : NULL;
	const COLFace* faces = model->getFaceCount() != 0 ? &model->getFaces()[0] : NULL;

	if (iptr == &sphereCountDummy) {
		if (role == Qt::DisplayRole) {
			return tr("Spheres");
		}
	} else if (iptr == &boxCountDummy) {
		if (role == Qt::DisplayRole) {
			return tr("Boxes");
		}
	} else if (iptr == &faceGroupCountDummy) {
		if (role == Qt::DisplayRole) {
			return tr("Face Groups");
		}
	} else if (iptr == &faceCountDummy) {
		if (role == Qt::DisplayRole) {
			return tr("Faces");
		}
	} else if (iptr >= fgs  &&  iptr <= fgs + (model->getFaceGroupCount()-1)) {
		if (role == Qt::DisplayRole) {
			return tr("Face Group %1").arg(((const COLFaceGroup*) iptr) - fgs);
		}
	} else if (iptr >= spheres  &&  iptr <= spheres + (model->getSphereCount()-1)) {
		if (role == Qt::DisplayRole) {
			return tr("Sphere %1").arg(((const COLSphere*) iptr) - spheres);
		}
	} else if (iptr >= boxes  &&  iptr <= boxes + (model->getBoxCount()-1)) {
		if (role == Qt::DisplayRole) {
			return tr("Box %1").arg(((const COLBox*) iptr) - boxes);
		}
	} else if (iptr >= faces  &&  iptr <= faces + (model->getFaceCount()-1)) {
		if (role == Qt::DisplayRole) {
			return tr("Face %1").arg(((const COLFace*) iptr) - faces);
		}
	}

	return QVariant();
}


QVariant COLEntityItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}


int COLEntityItemModel::rowCount(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return 3; // Spheres, Boxes, Faces/Face Groups
	}

	const COLFaceGroup* fgs = model->getFaceGroupCount() != 0 ? &model->getFaceGroups()[0] : NULL;

	void* iptr = index.internalPointer();

	if (iptr == &sphereCountDummy) {
		return model->getSphereCount();
	} else if (iptr == &boxCountDummy) {
		return model->getBoxCount();
	} else if (iptr == &faceGroupCountDummy) {
		return model->getFaceGroupCount();
	} else if (iptr == &faceCountDummy) {
		return model->getFaceCount();
	} else if (iptr >= fgs  &&  iptr <= fgs + (model->getFaceGroupCount()-1)) {
		return ((const COLFaceGroup*) iptr)->getFaceCount();
	}

	return 0;
}


QModelIndex COLEntityItemModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	const COLFaceGroup* fgs = model->getFaceGroupCount() != 0 ? &model->getFaceGroups()[0] : NULL;

	if (parent.isValid()) {
		void * pptr = parent.internalPointer();

		if (pptr == &sphereCountDummy) {
			return createIndex(row, column, &model->getSpheres()[0] + row);
		} else if (pptr == &boxCountDummy) {
			return createIndex(row, column, &model->getBoxes()[0] + row);
		} else if (pptr == &faceGroupCountDummy) {
			return createIndex(row, column, &model->getFaceGroups()[0] + row);
		} else if (pptr == &faceCountDummy) {
			return createIndex(row, column, &model->getFaces()[0] + row);
		} else if (pptr >= fgs  &&  pptr <= fgs + (model->getFaceGroupCount()-1)) {
			COLFaceGroup fg = *((const COLFaceGroup*) pptr);
			return createIndex(row, column, &model->getFaces()[0] + fg.getStartFace() + row);
		}
	} else {
		if (row == 0) {
			return createIndex(row, column, (void*) &sphereCountDummy);
		} else if (row == 1) {
			return createIndex(row, column, (void*) &boxCountDummy);
		} else {
			if (model->getFaceCount() != 0) {
				return createIndex(row, column, (void*) &faceGroupCountDummy);
			} else {
				return createIndex(row, column, (void*) &faceCountDummy);
			}
		}
	}

	return QModelIndex();
}


QModelIndex COLEntityItemModel::parent(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	const COLFaceGroup* fgs = model->getFaceGroupCount() != 0 ? &model->getFaceGroups()[0] : NULL;
	const COLSphere* spheres = model->getSphereCount() != 0 ? &model->getSpheres()[0] : NULL;
	const COLBox* boxes = model->getBoxCount() != 0 ? &model->getBoxes()[0] : NULL;
	const COLFace* faces = model->getFaceCount() != 0 ? &model->getFaces()[0] : NULL;

	void* iptr = index.internalPointer();

	if (iptr >= spheres  &&  iptr <= spheres + (model->getSphereCount()-1)) {
		return this->index(0, 0);
	} else if (iptr >= boxes  &&  iptr <= boxes + (model->getBoxCount()-1)) {
		return this->index(1, 0);
	} else if (iptr >= fgs  &&  iptr <= fgs + (model->getFaceGroupCount()-1)) {
		return this->index(2, 0);
	} else if (iptr >= faces  &&  iptr <= faces + (model->getFaceCount()-1)) {
		if (model->getFaceGroupCount() != 0) {
			uint32_t faceIdx = ((const COLFace*) iptr) - faces;

			for (uint32_t i = 0 ; i < model->getFaceGroupCount() ; i++) {
				COLFaceGroup fg = fgs[i];

				if (faceIdx >= fg.getStartFace()  &&  faceIdx <= fg.getEndFace()) {
					return createIndex(i, 0, (void*) (fgs + i));
				}
			}
		} else {
			return this->index(2, 0);
		}
	}

	return QModelIndex();
}


bool COLEntityItemModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
	if (role == Qt::CheckStateRole) {
		bool checked = data.toInt() == Qt::Checked;

		return true;
	}

	return false;
}


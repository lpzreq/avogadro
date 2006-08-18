/**********************************************************************
SphereRender - Renderer for van der Waals spheres

Copyright (C) 2006 by Geoffrey R. Hutchison
Some portions Copyright (C) 2006 by Donald E. Curtis

This file is part of the Avogadro molecular editor project.
For more information, see <http://avogadro.sourceforge.net/>

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

#ifndef __SPHERERENDER_H
#define __SPHERERENDER_H

#include <QGLWidget>
#include <openbabel/mol.h>

namespace Avogadro {

class SphereRender
{
 public:
 SphereRender() : dlist(0) {}
  ~SphereRender() {}

  GLuint Render(OpenBabel::OBMol &mol);

 private:
  GLuint dlist;

};

} // end namespace Avogadro

#endif

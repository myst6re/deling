/****************************************************************************
 ** Deling Final Fantasy VIII Field Editor
 ** Copyright (C) 2009-2024 Arzel Jérôme <myst6re@gmail.com>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include "ArgumentsScripts.h"

ArgumentsScripts::ArgumentsScripts() : CommonArguments()
{
	_ADD_FLAG("opcodes",
	          "Use the opcode listing instead of the pseudo-code.");
	_ADD_ARGUMENT("field", "Only process the field with this name (wildcards accepted). "
	                       "Repeat this argument to select multiple fields.", "field", "");
}

bool ArgumentsScripts::opcodes() const
{
	return _parser.isSet("opcodes");
}

QStringList ArgumentsScripts::fields() const
{
	return _parser.values("field");
}

"""
   Copyright (c) 2023 Wojciech Graj

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
"""

import sys


if __name__ == "__main__":
    filename = sys.argv[1]
    varname = f"{filename[3:].replace('.', '_').replace('/', '_').replace('-', '_')}"
    with open(filename, "rb") as f:
        data = [str(c) for c in f.read()]
        print(
            f"const unsigned char {varname}[]={{"
            + ','.join(data)
            + "};"
            + f"\nconst unsigned int {varname}_len={len(data)};"
        )

/////////////////////////////////////////////////////////////////////////
//
// ISABEL: A group collaboration tool for the Internet
// Copyright (C) 2009 Agora System S.A.
// 
// This file is part of Isabel.
// 
// Isabel is free software: you can redistribute it and/or modify
// it under the terms of the Affero GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Isabel is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Affero GNU General Public License for more details.
// 
// You should have received a copy of the Affero GNU General Public License
// along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
// $Id: tablas.cxx 20759 2010-07-05 10:30:36Z gabriel $
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#include "tablas.hh"


//
// Inicializacion de la tabla optimizada para codificar.
// En cada posicion apuntada por los indices
// [last] [run] [level]  esta el VLC (code) y su long (n_bits)
//
// Las combinaciones que no vienen en la tabla de la
// Recomendacion se distinguen por n_bits=0
//
Fila_TCOEF_cod Tabla_TCOEF::tabla_cod [2][41][13]= {
  {//last 0-----------------------------

    {//run 0
      {0 ,0},
      {2 ,0x2 },
      {4 ,0xf },
      {6 ,0x15},
      {7 ,0x17},
      {8 ,0x1f},
      {9 ,0x25},
      {9 ,0x24},
      {10,0x21},
      {10,0x20},
      {11,0x7 },
      {11,0x6 },
      {11,0x20},
    },

    {//run 1
      {0 ,0},
      {3 ,0x6 },
      {6 ,0x14},
      {8 ,0x1e},
      {10,0xf },
      {11,0x21},
      {12,0x50},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},

    },

    {//run 2
      {0 ,0},
      {4 ,0xe },
      {8 ,0x1d},
      {10,0xe },
      {12,0x51},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},

    },

    {//run 3
      {0 ,0},
      {5 ,0xd },
      {9 ,0x23},
      {10,0xd },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},

    },

    {//run 4
      {0 ,0},
      {5 ,0xc },
      {9 ,0x22},
      {12,0x52},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},


    },

    {//run 5
      {0 ,0},
      {5 ,0xb },
      {10,0xc },
      {12,0x53},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},


    },

    {//run 6
      {0 ,0},
      {6 ,0x13},
      {10,0xb },
      {12,0x54},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},


    },

    {//run 7
      {0 ,0},
      {6 ,0x12},
      {10,0xa },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},


    },

    {//run 8
      {0 ,0},
      {6 ,0x11},
      {10,0x9 },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},


    },

    {//run 9
      {0 ,0},
      {6 ,0x10},
      {10,0x8 },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},


    },

    {//run 10
      {0 ,0},
      {7 ,0x16},
      {12,0x52},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},


    },

    {//run 11
      {0 ,0},
      {7 ,0x15},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},


    },

    {//run 12
      {0 ,0},
      {7 ,0x14},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},

    },

    {//run 13
      {0 ,0},
      {8 ,0x1c},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 14
      {0 ,0},
      {8 ,0x1b},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 15
      {0 ,0},
      {9 ,0x21},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 16
      {0 ,0},
      {9 ,0x20},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 17
      {0 ,0},
      {9 ,0x1f},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 18
      {0 ,0},
      {9 ,0x1e},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 19
      {0 ,0},
      {9 ,0x1d},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 20
      {0 ,0},
      {9 ,0x1c},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 21
      {0 ,0},
      {9 ,0x1b},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 22
      {0 ,0},
      {9 ,0x1a},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 23
      {0 ,0},
      {11,0x22},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 24
      {0 ,0},
      {11,0x23},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 25
      {0 ,0},
      {12,0x56},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 26
      {0 ,0},
      {12,0x57},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },


    {//run 27
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 28
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 29
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 30
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 31
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 32
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 33
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 34
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 35
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 36
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 37
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 38
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 39
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 40
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

  }//last 0-----------------------------
,








  {//last 1-----------------------------

    {//run 0
      {0 ,0},
      {4 ,0x7 },
      {9 ,0x19},
      {11,0x5 },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},

    },

    {//run 1
      {0 ,0},
      {6 ,0xf },
      {11,0x4 },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 2
      {0 ,0},
      {6 ,0xe },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},


    },

    {//run 3
      {0 ,0},
      {6 ,0xd },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 4
      {0 ,0},
      {6 ,0xc } ,
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 5
      {0 ,0},
      {7 ,0x13},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 6
      {0 ,0},
      {7 ,0x12},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 7
      {0 ,0},
      {7 ,0x11},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 8
      {0 ,0},
      {7 ,0x10},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 9
      {0 ,0},
      {8 ,0x1a},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 10
      {0 ,0},
      {8 ,0x19},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 11
      {0 ,0},
      {8 ,0x18},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 12
      {0 ,0},
      {8 ,0x17},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 13
      {0 ,0},
      {8 ,0x16},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 14
      {0 ,0},
      {8 ,0x15},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 15
      {0 ,0},
      {8 ,0x14},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 16
      {0 ,0},
      {8 ,0x13},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 17
      {0 ,0},
      {9 ,0x18},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 18
      {0 ,0},
      {9 ,0x17},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 19
      {0 ,0},
      {9 ,0x16},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 20
      {0 ,0},
      {9 ,0x15},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 21
      {0 ,0},
      {9 ,0x14},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 22
      {0 ,0},
      {9 ,0x13},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 23
      {0 ,0},
      {9 ,0x12},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 24
      {0 ,0},
      {9 ,0x11},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 25
      {0 ,0},
      {10,0x7 },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 26
      {0 ,0},
      {10,0x6 },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 27
      {0 ,0},
      {10,0x5 },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
    },

    {//run 28
      {0 ,0},
      {10,0x4 },
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 29
      {0 ,0},
      {11,0x24},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 30
      {0 ,0},
      {11,0x25},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 31
      {0 ,0},
      {11,0x26},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 32
      {0 ,0},
      {11,0x27},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 33
      {0 ,0},
      {12,0x58},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
  },

    {//run 34
      {0 ,0},
      {12,0x59},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 35
      {0 ,0},
      {12,0x5a},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 36
      {0 ,0},
      {12,0x5b},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 37
      {0 ,0},
      {12,0x5c},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 38
      {0 ,0},
      {12,0x5d},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 39
      {0 ,0},
      {12,0x5e},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

    {//run 40
      {0 ,0},
      {12,0x5f},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
      {0 ,0},
   },

  }//last 1-----------------------------
};


int Tabla_TCOEF::max_index = 101;
Fila_TCOEF_dec Tabla_TCOEF::tabla_dec [] =
{  {2,  0x2, {0,0,1}},

   {3,  0x6, {0,1,1}},

   {4,  0xe, {0,2,1}},
   {4,  0xf, {0,0,2}},
   {4,  0x7, {1,0,1}},

   {5,  0xd, {0,3,1}},
   {5,  0xc, {0,4,1}},
   {5,  0xb, {0,5,1}},

   {6,  0x15, {0,0,3}},
   {6,  0x14, {0,1,2}},
   {6,  0x13, {0,6,1}},
   {6,  0x12, {0,7,1}},
   {6,  0x11, {0,8,1}},
   {6,  0x10, {0,9,1}},
   {6,  0xf, {1,1,1}},
   {6,  0xe, {1,2,1}},
   {6,  0xd, {1,3,1}},
   {6,  0xc, {1,4,1}},

   {7,  0x16, {0,10,1}},
   {7,  0x15, {0,11,1}},
   {7,  0x14, {0,12,1}},
   {7,  0x17, {0,0,4}},
   {7,  0x13, {1,5,1}},
   {7,  0x12, {1,6,1}},
   {7,  0x11, {1,7,1}},
   {7,  0x10, {1,8,1}},

   {8,  0x1f, {0,0,5}},
   {8,  0x1e, {0,1,3}},
   {8,  0x1d, {0,2,2}},
   {8,  0x1c, {0,13,1}},
   {8,  0x1b, {0,14,1}},
   {8,  0x1a, {1,9,1}},
   {8,  0x19, {1,10,1}},
   {8,  0x18, {1,11,1}},
   {8,  0x17, {1,12,1}},
   {8,  0x16, {1,13,1}},
   {8,  0x15, {1,14,1}},
   {8,  0x14, {1,15,1}},
   {8,  0x13, {1,16,1}},

   {9,  0x25, {0,0,6}},
   {9,  0x24, {0,0,7}},
   {9,  0x23, {0,3,2}},
   {9,  0x22, {0,4,2}},
   {9,  0x21, {0,15,1}},
   {9,  0x20, {0,16,1}},
   {9,  0x1f, {0,17,1}},
   {9,  0x1e, {0,18,1}},
   {9,  0x1d, {0,19,1}},
   {9,  0x1c, {0,20,1}},
   {9,  0x1b, {0,21,1}},
   {9,  0x1a, {0,22,1}},
   {9,  0x19, {1,0,2}},
   {9,  0x18, {1,17,1}},
   {9,  0x17, {1,18,1}},
   {9,  0x16, {1,19,1}},
   {9,  0x15, {1,20,1}},
   {9,  0x14, {1,21,1}},
   {9,  0x13, {1,22,1}},
   {9,  0x12, {1,23,1}},
   {9,  0x11, {1,24,1}},

   {10, 0x21, {0,0,8}},
   {10, 0x20, {0,0,9}},
   {10, 0xf, {0,1,4}},
   {10, 0xe, {0,2,3}},
   {10, 0xd, {0,3,3}},
   {10, 0xc, {0,5,2}},
   {10, 0xb, {0,6,2}},
   {10, 0xa, {0,7,2}},
   {10, 0x9, {0,8,2}},
   {10, 0x8, {0,9,2}},
   {10, 0x7, {1,25,1}},
   {10, 0x6, {1,26,1}},
   {10, 0x5, {1,27,1}},
   {10, 0x4, {1,28,1}},

   {11, 0x7, {0,0,10}},
   {11, 0x6, {0,0,11}},
   {11, 0x20, {0,0,12}},
   {11, 0x21, {0,1,5}},
   {11, 0x22, {0,23,1}},
   {11, 0x23, {0,24,1}},
   {11, 0x5, {1,0,3}},
   {11, 0x4, {1,1,2}},
   {11, 0x24, {1,29,1}},
   {11, 0x25, {1,30,1}},
   {11, 0x26, {1,31,1}},
   {11, 0x27, {1,32,1}},

   {12, 0x50, {0,1,6}},
   {12, 0x51, {0,2,4}},
   {12, 0x52, {0,4,3}},
   {12, 0x53, {0,5,3}},
   {12, 0x54, {0,6,3}},
   {12, 0x55, {0,10,2}},
   {12, 0x56, {0,25,1}},
   {12, 0x57, {0,26,1}},
   {12, 0x58, {1,33,1}},
   {12, 0x59, {1,34,1}},
   {12, 0x5a, {1,35,1}},
   {12, 0x5b, {1,36,1}},
   {12, 0x5c, {1,37,1}},
   {12, 0x5d, {1,38,1}},
   {12, 0x5e, {1,39,1}},
   {12, 0x5f, {1,40,1}}
};


/*******************************************************
Busca en la tabla los siguientes bits del lector y mete
la informacion asociada en 'inf' .

Develve: 0 si todo va bien
         1 si los bits que siguen no estan en la tabla
*******************************************************/
int
Tabla_TCOEF::obtener_info (Lector &lector, Inf_TCOEF &inf)
{
    int i;
    u8 n_bits;
    u8 n_bits_old=0;
    u16 code=0;

    //
    // Comprobar si es ESCAPE
    //
    if (lector.lee_bits (7) == 0x3) {
#ifdef DEBUG
        fprintf(stderr,"ESCAPE\n");
#endif
        inf.last = lector.lee_bits(1);
        inf.run = lector.lee_bits(6);
        inf.level = (i8) lector.lee_bits(8);
        inf.sign = (inf.level < 0);

        //Guardamos el valor absoluto
        inf.level = abs (inf.level);

        return 0;
    } else {
        lector.devuelve_bits (7);
        for (i=0; i <= Tabla_TCOEF::max_index; i++) {
            n_bits = tabla_dec[i].n_bits;
            if (n_bits != n_bits_old) {
                code <<= (n_bits - n_bits_old);
                code |= lector.lee_bits (n_bits - n_bits_old);
                n_bits_old = n_bits;
            }
            if (tabla_dec[i].code == code) {
                inf=tabla_dec[i].informacion;
                inf.sign=lector.lee_bits (1);
                return 0;
            }
        }
    }

    //
    // Si llegamos aqui es que los ultimos bits no estan en la tabla
    //
    fprintf(stderr,"Tabla_TCOEF::obtener_info: Code TCOEF NO encontrado\n");

    return 1;
}



/*******************************************************
Busca en la tabla la informacion asociada en 'inf'
y manda al escritor los bits del codigo VLC.

Develve: 0 si todo va bien
         1 si la informacion no esta en la tabla
*******************************************************/
int
Tabla_TCOEF::escribir_info (Escritor &escritor, Inf_TCOEF inf)
{
    u8 n_bits;
    u16 code;

    if ( (inf.run <= 40) && (inf.level <= 12) ) {
        if ( (n_bits=tabla_cod [inf.last] [inf.run] [inf.level].n_bits) != 0) {
            code = tabla_cod [inf.last] [inf.run] [inf.level].code;
            escritor.escribe_bits (code, n_bits);
            if (inf.sign) {
                escritor.escribe_bits (1, 1);
            } else {
                escritor.escribe_bits (0, 1);
            }
            return 0;
        }
    }

    //ESCAPE
    escritor.escribe_bits (0x3, 7);
    // Last
    escritor.escribe_bits (inf.last, 1);
    // Run
    escritor.escribe_bits (inf.run, 6);
    // Level, incluido ya el signo
    if (inf.sign) {
        inf.level *= (-1);
    }
    escritor.escribe_bits (inf.level, 8);

    return 0;
}


/*******************************************************/


int Tabla_MCBPC_for_I::max_index = 8;
Fila_MCBPC Tabla_MCBPC_for_I::tabla [] =
{ {1, 0x1, {'3',0x0}},

  {3, 0x1, {'3',0x1}},
  {3, 0x2, {'3',0x2}},
  {3, 0x3, {'3',0x3}},

  {4, 0x1, {'4',0x0}},

  {6, 0x1, {'4',0x1}},
  {6, 0x2, {'4',0x2}},
  {6, 0x3, {'4',0x3}},

  {9, 0x1, {'s',0x0}}  //MBtype=stuffing
};



/*******************************************************
Busca en la tabla los siguientes bits del lector y mete
la informacion asociada en 'inf' .

Develve: 0 si todo va bien
         1 si los bits que siguen no estan en la tabla
*******************************************************/
int
Tabla_MCBPC_for_I::obtener_info (Lector &lector, Inf_MCBPC &inf)
{
    int i;
    u8 n_bits;
    u8 n_bits_old=0;
    u16 code=0;

    for (i=0; i <= Tabla_MCBPC_for_I::max_index; i++) {
        n_bits = tabla[i].n_bits;
        if (n_bits != n_bits_old) {
            code <<= (n_bits - n_bits_old);
            code |= lector.lee_bits (n_bits - n_bits_old);
            n_bits_old = n_bits;
        }
        if (tabla[i].code == code) {
            inf=tabla[i].informacion;
            return 0;
        }
    }


    //
    // Si llegamos aqui es que los ultimos bits no estan en la tabla
    //
    fprintf(stderr,
            "Tabla_MCBPC_for_I::obtener_info: Code MCBPC_for_I NO encontrado\n"
           );

    return 1;
}


/*******************************************************
Busca en la tabla la informacion asociada en 'inf'
y manda al escritor los bits del codigo VLC.

Develve: 0 si todo va bien
         1 si la informacion no esta en la tabla
*******************************************************/
int
Tabla_MCBPC_for_I::escribir_info (Escritor &escritor, Inf_MCBPC inf)
{
    int i;
    u8 n_bits;
    u16 code;

    for (i=0; i <= Tabla_MCBPC_for_I::max_index; i++) {
        n_bits = tabla[i].n_bits;
        code = tabla[i].code;
        if ( (tabla[i].informacion.MBtype == inf.MBtype) &&
             (tabla[i].informacion.CBPC == inf.CBPC) ) {
            escritor.escribe_bits (code, n_bits);
            return 0;
        }
    }

    //
    // Si llegamos aqui es que la informacion no esta en la tabla
    //
    fprintf(stderr,
            "Tabla_MCBPC_for_I::escribir_info: Inf MCBPC_for_I NO encontrado\n"
           );

    return 1;
}


/*******************************************************/

int Tabla_MCBPC_for_P::max_index = 20;
Fila_MCBPC Tabla_MCBPC_for_P::tabla [] =
{  {1, 0x1, {'0',0x0}},

   {3, 0x3, {'1',0x0}},
   {3, 0x2, {'2',0x0}},

   {4, 0x3, {'0',0x1}},
   {4, 0x2, {'0',0x2}},

   {5, 0x3, {'3',0x0}},

   {6, 0x5, {'0',0x3}},
   {6, 0x4, {'4',0x0}},

   {7, 0x7, {'1',0x1}},
   {7, 0x6, {'1',0x2}},
   {7, 0x5, {'2',0x1}},
   {7, 0x4, {'2',0x2}},
   {7, 0x3, {'3',0x3}},

   {8, 0x5, {'2',0x3}},
   {8, 0x4, {'3',0x1}},
   {8, 0x3, {'3',0x2}},

   {9, 0x5, {'1',0x3}},
   {9, 0x4, {'4',0x1}},
   {9, 0x3, {'4',0x2}},
   {9, 0x2, {'4',0x3}},
   {9, 0x1, {'s',0x0}}  //MBtype=stuffing
};


/*******************************************************
Busca en la tabla los siguientes bits del lector y mete
la informacion asociada en 'inf' .

Develve: 0 si todo va bien
         1 si los bits que siguen no estan en la tabla
*******************************************************/
int
Tabla_MCBPC_for_P::obtener_info (Lector &lector, Inf_MCBPC &inf)
{
    int i;
    u8 n_bits;
    u8 n_bits_old=0;
    u16 code=0;

    for (i=0; i <= Tabla_MCBPC_for_P::max_index; i++) {
        n_bits = tabla[i].n_bits;
        if (n_bits != n_bits_old) {
            code <<= (n_bits - n_bits_old);
            code |= lector.lee_bits (n_bits - n_bits_old);
            n_bits_old = n_bits;
        }
        if (tabla[i].code == code) {
            inf=tabla[i].informacion;
            return 0;
        }
    }

    //
    // Si llegamos aqui es que los ultimos bits no estan en la tabla
    //
    fprintf(stderr,
            "Tabla_MCBPC_for_P::obtener_info: Code MCBPC_for_P NO encontrado\n"
           );

    return 1;
}

/*******************************************************
Busca en la tabla la informacion asociada en 'inf'
y manda al escritor los bits del codigo VLC.

Develve: 0 si todo va bien
         1 si la informacion no esta en la tabla
*******************************************************/
int
Tabla_MCBPC_for_P::escribir_info (Escritor &escritor, Inf_MCBPC inf)
{
    int i;
    u8 n_bits;
    u16 code;

    for (i=0; i <= Tabla_MCBPC_for_P::max_index; i++) {
        n_bits = tabla[i].n_bits;
        code = tabla[i].code;
        if ( (tabla[i].informacion.MBtype == inf.MBtype) &&
             (tabla[i].informacion.CBPC == inf.CBPC) ) {
            escritor.escribe_bits (code, n_bits);
            return 0;
        }
    }

    //
    // Si llegamos aqui es que la informacion no esta en la tabla
    //
    fprintf(stderr,
            "Tabla_MCBPC_for_P::escribir_info: Inf MCBPC_for_P NO encontrado\n"
           );

    return 1;
}


/*******************************************************/


int Tabla_CBPY::max_index = 15;
Fila_CBPY Tabla_CBPY::tabla [] =
{
   {2, 0x3, {0xf, 0x0}},

   {4, 0x3, {0x0, 0xf}},
   {4, 0x9, {0x3, 0xc}},
   {4, 0x7, {0x5, 0xa}},
   {4, 0xb, {0x7, 0x8}},
   {4, 0x5, {0xa, 0x5}},
   {4, 0xa, {0xb, 0x4}},
   {4, 0x4, {0xc, 0x3}},
   {4, 0x8, {0xd, 0x2}},
   {4, 0x6, {0xe, 0x1}},

   {5, 0x5, {0x1, 0xe}},
   {5, 0x4, {0x2, 0xd}},
   {5, 0x3, {0x4, 0xb}},
   {5, 0x2, {0x8, 0x7}},

   {6, 0x2, {0x6, 0x9}},
   {6, 0x3, {0x9, 0x6}}
};



/*******************************************************
Busca en la tabla los siguientes bits del lector y mete
la informacion asociada en 'inf' .

Develve: 0 si todo va bien
         1 si los bits que siguen no estan en la tabla
*******************************************************/
int
Tabla_CBPY::obtener_info (Lector &lector, Inf_CBPY &inf)
{
    int i;
    u8 n_bits;
    u8 n_bits_old=0;
    u8 code=0;

    for (i=0; i <= Tabla_CBPY::max_index; i++) {
        n_bits = tabla[i].n_bits;
        if (n_bits != n_bits_old) {
            code <<= (n_bits - n_bits_old);
            code |= lector.lee_bits (n_bits - n_bits_old);
            n_bits_old = n_bits;
        }
        if (tabla[i].code == code) {
            inf=tabla[i].informacion;
            return 0;
        }
    }

    //
    // Si llegamos aqui es que los ultimos bits no estan en la tabla
    //
    fprintf(stderr,"Tabla_CBPY::obtener_info: Code CBPY NO encontrado\n");

    return 1;
}


/*******************************************************
Busca en la tabla la informacion asociada en 'inf'
y manda al escritor los bits del codigo VLC.

Develve: 0 si todo va bien
         1 si la informacion no esta en la tabla
*******************************************************/
int
Tabla_CBPY::escribir_info_for_I (Escritor &escritor, Inf_CBPY inf)
{
    int i;
    u8 n_bits;
    u8 code;

    for (i=0; i <= Tabla_CBPY::max_index; i++) {
        n_bits = tabla[i].n_bits;
        code = tabla[i].code;
        if (tabla[i].informacion.CBPY_I == inf.CBPY_I) {
            escritor.escribe_bits (code, n_bits);
            return 0;
        }
    }

    //
    // Si llegamos aqui es que la informacion no esta en la tabla
    //
    fprintf(stderr,"Tabla_CBPY::escribir_info_for_I: Inf CBPY NO encontrado\n");

    return 1;
}

/*******************************************************
Busca en la tabla la informacion asociada en 'inf'
y manda al escritor los bits del codigo VLC.

Develve: 0 si todo va bien
         1 si la informacion no esta en la tabla
*******************************************************/
int
Tabla_CBPY::escribir_info_for_P (Escritor &escritor, Inf_CBPY inf)
{
    int i;
    u8 n_bits;
    u8 code;

    for (i=0; i <= Tabla_CBPY::max_index; i++) {
        n_bits = tabla[i].n_bits;
        code = tabla[i].code;
        if (tabla[i].informacion.CBPY_P == inf.CBPY_P) {
            escritor.escribe_bits (code, n_bits);
            return 0;
        }
    }

    //
    // Si llegamos aqui es que la informacion no esta en la tabla
    //
    fprintf(stderr,"Tabla_CBPY::escribir_info_for_P: Inf CBPY NO encontrado\n");

    return 1;
}



/*******************************************************/

int Tabla_MVD::max_index = 63;
Fila_MVD Tabla_MVD::tabla [] =
{  {13, 0x5 , {-32, 32}},
   {13, 0x7 , {-31, 33}},
   {12, 0x5 , {-30, 34}},
   {12, 0x7 , {-29, 35}},
   {12, 0x9 , {-28, 36}},
   {12, 0xb , {-27, 37}},
   {12, 0xd , {-26, 38}},
   {12, 0xf , {-25, 39}},
   {11, 0x9 , {-24, 40}},
   {11, 0xb , {-23, 41}},
   {11, 0xd , {-22, 42}},
   {11, 0xf , {-21, 43}},
   {11, 0x11, {-20, 44}},
   {11, 0x13, {-19, 45}},
   {11, 0x15, {-18, 46}},
   {11, 0x17, {-17, 47}},
   {11, 0x19, {-16, 48}},
   {11, 0x1b, {-15, 49}},
   {11, 0x1d, {-14, 50}},
   {11, 0x1f, {-13, 51}},
   {11, 0x21, {-12, 52}},
   {11, 0x23, {-11, 53}},
   {10, 0x13, {-10, 54}},
   {10, 0x15, {-9,  55}},
   {10, 0x17, {-8,  56}},
   {8 , 0x7 , {-7,  57}},
   {8 , 0x9 , {-6,  58}},
   {8 , 0xb , {-5,  59}},
   {7 , 0x7 , {-4,  60}},
   {5 , 0x3 , {-3,  61}},
   {4 , 0x3 , {-2,  62}},
   {3 , 0x3 , {-1,  63}},
   {1 , 0x1 , {0,     0}},
   {3 , 0x2 , {1,   -63}},
   {4 , 0x2 , {2,   -62}},
   {5 , 0x2 , {3,   -61}},
   {7 , 0x6 , {4,   -60}},
   {8 , 0xa , {5,   -59}},
   {8 , 0x8 , {6,   -58}},
   {8 , 0x6 , {7,   -57}},
   {10, 0x16, {8,   -56}},
   {10, 0x14, {9,   -55}},
   {10, 0x12, {10,  -54}},
   {11, 0x22, {11,  -53}},
   {11, 0x20, {12,  -52}},
   {11, 0x1e, {13,  -51}},
   {11, 0x1c, {14,  -50}},
   {11, 0x1a, {15,  -49}},
   {11, 0x18, {16,  -48}},
   {11, 0x16, {17,  -47}},
   {11, 0x14, {18,  -46}},
   {11, 0x12, {19,  -45}},
   {11, 0x10, {20,  -44}},
   {11, 0xe , {21,  -43}},
   {11, 0xc , {22,  -42}},
   {11, 0xa , {23,  -41}},
   {11, 0x8 , {24,  -40}},
   {12, 0xe , {25,  -39}},
   {12, 0xc , {26,  -38}},
   {12, 0xa , {27,  -37}},
   {12, 0x8 , {28,  -36}},
   {12, 0x6 , {29,  -35}},
   {12, 0x4 , {30,  -34}},
   {13, 0x6 , {31,  -33}}
};


/*******************************************************
Busca en la tabla los siguientes bits del lector y mete
la informacion asociada en 'inf' .

Develve: 0 si todo va bien
         1 si los bits que siguen no estan en la tabla
*******************************************************/
int
Tabla_MVD::obtener_info (Lector &lector, Inf_MVD &inf)
{
    int i;
    u8 n_bits;

    for (i=0; i <= Tabla_MVD::max_index; i++) {
        n_bits = tabla[i].n_bits;
        if (tabla[i].code == lector.lee_bits (n_bits)) {
            inf=tabla[i].informacion;
            return 0;
        } else {
            lector.devuelve_bits (n_bits);
        }
    }

    //
    // Si llegamos aqui es que los ultimos bits no estan en la tabla
    //
    fprintf(stderr,"Tabla_MVD::obtener_info: Code MVD NO encontrado\n");

    return 1;
}



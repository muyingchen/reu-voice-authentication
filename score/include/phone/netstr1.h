/* file netstr1.h                         */
/* structure #1 for representing networks */
/* composed of nodes and arcs.            */

struct ARC1
{char *symbol;
 int isymb;
 struct NODE1 *from_node, *to_node;
};
typedef struct ARC1 ARC;

struct ARC_LIST_ATOM1
  {ARC *arc; /* contents of this atom */
   struct ARC_LIST_ATOM1 *next, *prev;
  };
typedef struct ARC_LIST_ATOM1 ARC_LIST_ATOM;

struct NODE1
  {char *name;
   ARC_LIST_ATOM *in_arcs, *out_arcs;
   boolean start_state, stop_state, flag1, flag2;
  };
typedef struct NODE1 NODE;


 struct NETWORK1
   {char *name;
    int highest_nnode_name;
    NODE *start_node;
    NODE *stop_node;
   };
 typedef struct NETWORK1 NETWORK;

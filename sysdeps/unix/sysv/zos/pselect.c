#include <misc/pselect.c>

link_warning (pselect,
	      "pselect is known to have a race conditon, use select if "
	      "possible.")

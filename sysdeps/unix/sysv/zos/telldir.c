/* telldir stub
 * z/OS TODO: currently we aren't using a readdir syscall which would
 * allow us to have a notion where we are in a list of directories, or
 * a way to jump to a location in that list. The z/OS syscall
 * 'readdir2' has an indexed mode which should allow us to both have
 * an index and, possibly jump to an index. The details of that
 * syscall, and weather calls to readdir2 and readdir may be
 * intermingled, is unclear. Also calling readdir2 (vs readdir) has
 * performance implication according to the manual.
 */
#include <dirent/telldir.c>

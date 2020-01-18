#include "State.h"

bool Face::operator==(const Face &rhs) const { return Vec3<int>::operator==(rhs) && side == rhs.side; }

bool Face::operator!=(const Face &rhs) const { return Vec3<int>::operator!=(rhs) || side != rhs.side; }

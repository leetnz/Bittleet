
#include "Command.h"

namespace Command {

bool Command::operator!=(const Simple& other) const {
    return ((operator==(other) == false));
}

bool Command::operator==(const Simple& other) const {
    if (Type::Simple == _type) {
        return (other == _simple);
    } 
    return false;
}


bool Command::operator!=(const Command& other) const {
    return ((operator==(other) == false));
}

bool Command::operator==(const Command& other) const {
    if (other._type == _type) {
        switch (_type) {
            case Type::None:
                return true;
            case Type::Simple:
                return (other._simple == _simple);
            case Type::Move:
                return ((other._move.direction == _move.direction) && (other._move.pace == _move.pace));
            case Type::WithArgs:
                if (other._withArgs.len != _withArgs.len) {
                    return false;
                }
                for (uint8_t i = 0; i < _withArgs.len; i++) {
                    if (other._withArgs.args[i] != _withArgs.args[i]) {
                        return false;
                    }
                }
                return (other._withArgs.cmd == _withArgs.cmd);
            default:
                return false;
        }
    } 
    return false;
}


bool Command::get(Simple& cmd) {
    if (_type == Type::Simple) {
        cmd = _simple;
        return true;
    }
    return false;
}

bool Command::get(Move& cmd) {
    if (_type == Type::Move) {
        cmd = _move;
        return true;
    }
    return false;
}

bool Command::get(WithArgs& cmd) {
    if (_type == Type::WithArgs) {
        cmd = _withArgs;
        return true;
    }
    return false;
}


}


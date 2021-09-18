//
// Bittleet Commands
// Provides Structures Commands for Bittle
//
// Hoani Bryson (github.com/hoani)
// Copyright (c) 2021 Leetware Limited.
// License - MIT
//

#ifndef _BITTLEET_COMMANDS_H_
#define _BITTLEET_COMMANDS_H_

#include<Arduino.h>

namespace Command {

enum class Simple : uint8_t {
    None = 0,
    Rest,
    GyroToggle,
    Balance,
    Pause,
    Step,
    Sit,
    Stretch,
    Greet,
    Pushup,
    Hydrant,
    Check,
    Dead,
    Zero,
    Lifted,
    Dropped,
    Recover,
    SaveServoCalibration,
    AbortServoCalibration,
    ShowJointAngles,
    ShowHelp,
    TOTAL
};

enum class Pace : uint8_t {
    Slow,
    Medium,
    Fast,
    Reverse,
    TOTAL
};

enum class Direction : uint8_t {
    Forward = 0,
    Left,
    Right,
    TOTAL
};

enum class ArgType : uint8_t {
    Calibrate = 0,
    MoveSequentially,
    MoveSimultaneously,
    Meow,
    Beep,
    TOTAL
};

# define COMMAND_MAX_ARGS (16)
struct WithArgs {
    ArgType cmd;
    uint8_t len;
    int8_t args[COMMAND_MAX_ARGS];
};

struct Move {
    Pace pace;
    Direction direction;
};

enum class Type {
    None = 0,
    Simple,
    Move,
    WithArgs,
};

class Command {
    public:
        Command() = default;
        explicit Command(const Simple& cmd) : _type(Type::Simple), _simple(cmd) {};
        explicit Command(const Move& cmd) : _type(Type::Move), _move(cmd) {};
        explicit Command(const WithArgs& cmd) : _type(Type::WithArgs), _withArgs(cmd) {};
        Command(const Direction& cmd, const Move& lastMove);
        Command(const Pace& cmd, const Move& lastMove);

        bool operator!=(const Simple& other) const;
        bool operator==(const Simple& other) const;

        bool operator!=(const Command& other) const;
        bool operator==(const Command& other) const;

        bool get(Simple& cmd);
        bool get(Move& cmd);
        bool get(WithArgs& cmd);
        Type type() { return _type; }

    private:
        Type _type = Type::None;
        Simple _simple = Simple::None;
        Move _move = {Pace::Medium, Direction::Forward};
        WithArgs _withArgs = {ArgType::Beep, 0, {}};
};

} // namespace Command

#endif // _BITTLEET_COMMANDS_H_


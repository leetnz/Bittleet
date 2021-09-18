#ifndef _BITTLEET_COMMANDS_H_
#define  _BITTLEET_COMMANDS_H_

#include<Arduino.h>


#define CMD_REST        "d"       //rest and shutdown all servos 
#define CMD_FORWARD     "F"       //forward
#define CMD_GYRO_OFF    "g"       //turn off gyro feedback to boost speed

#define CMD_LEFT        "L"       //left
#define CMD_BALANCE     "balance" //neutral stand up posture
#define CMD_RIGHT       "R"       //right

#define CMD_PAUSE       "p"       //pause motion and shut off all servos 
#define CMD_BACKWARD    "B"       //backward
#define CMD_CALIBRATE   "c"       //calibration mode with IMU turned off

#define CMD_STEP        "vt"      //stepping
#define CMD_CRAWL       "cr"      //crawl
#define CMD_WALK        "wk"      //walk

#define CMD_TROT        "tr"      //trot
#define CMD_SIT         "sit"     //sit
#define CMD_STRETCH     "str"     //stretch

#define CMD_GREET       "hi"      //greeting
#define CMD_PUSHUP      "pu"      //push up
#define CMD_HYDRANT     "pee"     //standng with three legs

#define CMD_CHECK       "ck"      //check around
#define CMD_DEAD        "pd"      //play dead
#define CMD_ZERO        "zero"    //zero position

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

struct WithArgs {
    ArgType cmd;
    uint8_t len;
    int8_t args[16];
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


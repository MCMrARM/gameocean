#pragma once

class AnimationProperty {
public:
    virtual void set(float animStep) = 0;
};
class ByteAnimationProperty : public AnimationProperty {
private:
    char& property;
    char from, to;
public:
    ByteAnimationProperty(char& property, char from, char to) : property(property), from(from), to(to) { };
    virtual void set(float animStep) {
        property = from + (to - from) * animStep;
    }
};
class ShortAnimationProperty : public AnimationProperty {
private:
    short& property;
    short from, to;
public:
    ShortAnimationProperty(short& property, short from, short to) : property(property), from(from), to(to) { };
    virtual void set(float animStep) {
        property = from + (to - from) * animStep;
    }
};
class IntAnimationProperty : public AnimationProperty {
private:
    int& property;
    int from, to;
public:
    IntAnimationProperty(int& property, int from, int to) : property(property), from(from), to(to) { };
    virtual void set(float animStep) {
        property = from + (to - from) * animStep;
    }
};
class LongAnimationProperty : public AnimationProperty {
private:
    long long& property;
    long long from, to;
public:
    LongAnimationProperty(long long& property, long long from, long long to) : property(property), from(from), to(to) { };
    virtual void set(float animStep) {
        property = from + (to - from) * animStep;
    }
};
class FloatAnimationProperty : public AnimationProperty {
private:
    float& property;
    float from, to;
public:
    FloatAnimationProperty(float& property, float from, float to) : property(property), from(from), to(to) { };
    virtual void set(float animStep) {
        property = from + (to - from) * animStep;
    }
};
class DoubleAnimationProperty : public AnimationProperty {
private:
    double& property;
    double from, to;
public:
    DoubleAnimationProperty(double& property, double from, double to) : property(property), from(from), to(to) { };
    virtual void set(float animStep) {
        property = from + (to - from) * animStep;
    }
};
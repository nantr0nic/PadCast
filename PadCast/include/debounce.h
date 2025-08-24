class DebounceTimer
{
private:
    float debounceTime;
    float lastPressedTime;

public:
    DebounceTimer(float debounceSeconds = 0.5f) // 500ms
        : debounceTime(debounceSeconds), lastPressedTime(0.0f) {
    }

    bool CanAcceptInput()
    {
        float currentTime = GetTime(); // raylib's GetTime() returns seconds
        if (currentTime - lastPressedTime >= debounceTime)
        {
            lastPressedTime = currentTime;
            return true;
        }
        return false;
    }

    void Reset()
    {
        lastPressedTime = 0.0f;
    }
};
// Please read Bounce2.h for information about the liscence and authors


#include "Bounce2.h"

static const uint8_t DEBOUNCED_STATE = 0b00000001;
static const uint8_t UNSTABLE_STATE  = 0b00000010;
static const uint8_t CHANGED_STATE   = 0b00000100;


Bounce::Bounce()
    : previous_millis(0)
    , interval_millis(10)
    , state(0)
    , pin(0)
{}

void Bounce::attach(int pin) {
    this->pin = pin;
	bool currentState = readCurrentState();
	attach(currentState);
}

void Bounce::attach(int pin, int mode) {
	setPinMode(pin, mode);
	this->attach(pin);
}

void Bounce::attach(bool currentState) {
	state = 0;
	if (currentState) {
		setStateFlag(DEBOUNCED_STATE | UNSTABLE_STATE);
	}
#ifdef BOUNCE_LOCK_OUT
	previous_millis = 0;
#else
	previous_millis = millis();
#endif
}

void Bounce::interval(uint16_t interval_millis)
{
    this->interval_millis = interval_millis;
}

bool Bounce::update()
{
	// Read the state of the switch in a temporary variable.
	bool currentState = readCurrentState();

	update(currentState);
}

bool Bounce::update(bool currentState)
{

    unsetStateFlag(CHANGED_STATE);
#ifdef BOUNCE_LOCK_OUT
    
    // Ignore everything if we are locked out
    if (millis() - previous_millis >= interval_millis) {
        if ( currentState != getStateFlag(DEBOUNCED_STATE) ) {
            previous_millis = millis();
            changeState();
        }
    }
    

#elif defined BOUNCE_WITH_PROMPT_DETECTION

    if ( readState != getStateFlag(DEBOUNCED_STATE) ) {
      // We have seen a change from the current button state.

      if ( millis() - previous_millis >= interval_millis ) {
	// We have passed the time threshold, so a new change of state is allowed.
	// set the STATE_CHANGED flag and the new DEBOUNCED_STATE.
	// This will be prompt as long as there has been greater than interval_misllis ms since last change of input.
	// Otherwise debounced state will not change again until bouncing is stable for the timeout period.
		 changeState();
      }
    }

    // If the readState is different from previous readState, reset the debounce timer - as input is still unstable
    // and we want to prevent new button state changes until the previous one has remained stable for the timeout.
    if ( readState != getStateFlag(UNSTABLE_STATE) ) {
	// Update Unstable Bit to macth readState
        toggleStateFlag(UNSTABLE_STATE);
        previous_millis = millis();
    }
    
    
#else
    // If the reading is different from last reading, reset the debounce counter
    if ( currentState != getStateFlag(UNSTABLE_STATE) ) {
        previous_millis = millis();
         toggleStateFlag(UNSTABLE_STATE);
    } else
        if ( millis() - previous_millis >= interval_millis ) {
            // We have passed the threshold time, so the input is now stable
            // If it is different from last state, set the STATE_CHANGED flag
            if (currentState != getStateFlag(DEBOUNCED_STATE) ) {
                previous_millis = millis();
                 

                 changeState();
            }
        }

    
#endif

		return  getStateFlag(CHANGED_STATE); 

}

// WIP HELD
unsigned long Bounce::previousDuration() {
	return durationOfPreviousState;
}

unsigned long Bounce::duration() {
	return (millis() - stateChangeLastTime);
}

inline void Bounce::changeState() {
	toggleStateFlag(DEBOUNCED_STATE);
	setStateFlag(CHANGED_STATE) ;
	durationOfPreviousState = millis() - stateChangeLastTime;
	stateChangeLastTime = millis();
}

bool Bounce::read()
{
    return  getStateFlag(DEBOUNCED_STATE);
}

bool Bounce::rose()
{
    return getStateFlag(DEBOUNCED_STATE) && getStateFlag(CHANGED_STATE);
}

bool Bounce::fell()
{
    return  !getStateFlag(DEBOUNCED_STATE) && getStateFlag(CHANGED_STATE);
}

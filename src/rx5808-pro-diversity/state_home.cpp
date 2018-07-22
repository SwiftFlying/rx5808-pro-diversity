#include <avr/pgmspace.h>

#include "settings_eeprom.h"

#include "state_home.h"

#include "receiver.h"
#include "channels.h"
#include "buttons.h"
#include "state.h"
#include "ui.h"

using StateMachine::HomeStateHandler;

void HomeStateHandler::onEnter() {
    Ui::clear();
}

void HomeStateHandler::onUpdate() {
    Ui::needUpdate();
}

void HomeStateHandler::onInitialDraw() {

    Ui::setTextSize(5);
    Ui::setTextColor(WHITE);
    Ui::setCursor(0, 0);

    Ui::display.print(Channels::getName(Receiver::activeChannel));
    
    Ui::setCursor(0, 30);
    Ui::display.print(Channels::getFrequency(Receiver::activeChannel));

    float scaling = float(SCREEN_HEIGHT - 1) / 100.0;
    uint8_t rssiAHeight = Receiver::rssiA * scaling;
    uint8_t rssiBHeight = Receiver::rssiB * scaling;
    uint8_t rssiCHeight = Receiver::rssiC * scaling;
    uint8_t rssiDHeight = Receiver::rssiD * scaling;

    
    Ui::clearRect(SCREEN_WIDTH - 40, 0, 39, SCREEN_HEIGHT-1);
    for (int i = 0; i<9; i++) {
      Ui::drawFastVLine(SCREEN_WIDTH - 40 + i, SCREEN_HEIGHT - 1 - rssiAHeight, rssiAHeight, WHITE);
      Ui::drawFastVLine(SCREEN_WIDTH - 30 + i, SCREEN_HEIGHT - 1 - rssiBHeight, rssiBHeight, WHITE);
      Ui::drawFastVLine(SCREEN_WIDTH - 20 + i, SCREEN_HEIGHT - 1 - rssiCHeight, rssiCHeight, WHITE);
      Ui::drawFastVLine(SCREEN_WIDTH - 10 + i, SCREEN_HEIGHT - 1 - rssiDHeight, rssiDHeight, WHITE);
    }
      
}

void HomeStateHandler::onUpdateDraw() {
    this->onInitialDraw();
}

void HomeStateHandler::onButtonChange(
    Button button,
    Buttons::PressType pressType
) {
  
  if (
      pressType == Buttons::PressType::SHORT &&
      button == Button::MODE_PRESSED
     ) {
        this->setChannel(8);
        }
  else if (
      pressType == Buttons::PressType::LONG &&
      button == Button::MODE_PRESSED
     ) {
        this->setChannel(-8);
        }
  else if (
      pressType == Buttons::PressType::SHORT &&
      button == Button::UP_PRESSED
     ) {
        this->setChannel(1);
        }
  else if (
      pressType == Buttons::PressType::SHORT &&
      button == Button::DOWN_PRESSED
     ) {
        this->setChannel(-1);
        }
    
}

void HomeStateHandler::setChannel(int channelIncrement) {

    int band = Receiver::activeChannel / 8;
    int channel = Receiver::activeChannel % 8;
    
    if (channelIncrement == 8) {
      band = band + 1;
    }
    
    if (channelIncrement == -8) {
      band = band - 1;
    }
    
    if (channelIncrement == 1 ) {
      channel = channel + 1;
      if (channel > 7) {
        channel = 0;
      }
    }
    
    if (channelIncrement == -1 ) {
      channel = channel - 1;
      if (channel < 0) {
        channel = 7;
      }
    }
    
    int newChannelIndex = band * 8 + channel;

    if (newChannelIndex >= CHANNELS_SIZE) {
      newChannelIndex = newChannelIndex - CHANNELS_SIZE;
    }
    if (newChannelIndex < 0) {
      newChannelIndex = newChannelIndex + CHANNELS_SIZE;
    }

    Receiver::setChannel(newChannelIndex);
    EepromSettings.startChannel = newChannelIndex;
    EepromSettings.markDirty();
}

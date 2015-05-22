#import "KeyboardView.h"

#include "Logger.h"
#include "KeyboardHandler.h"

@implementation KeyboardView {
    std::string currentText;
}

- (BOOL) canBecomeFirstResponder
{
    return YES;
}

- (void) setText: (std::string)text
{
    currentText = text;
}

- (void) insertText: (NSString*)text
{
    if([text isEqualToString:@"\n"]) {
        [self resignFirstResponder];
        return;
    }
    currentText += std::string(text.UTF8String);
    Logger::main->trace("Keyboard", "New text: %s", currentText.c_str());
    KeyboardHandler::inputSetText(currentText);
}

- (void) deleteBackward
{
    if(currentText.size() <= 0) return;
    currentText.erase(currentText.begin() + currentText.size() - 1);
    Logger::main->trace("Keyboard", "New text: %s", currentText.c_str());
    KeyboardHandler::inputSetText(currentText);
}

- (BOOL) hasText
{
    return YES;
}

- (UIReturnKeyType) returnKeyType
{
	return UIReturnKeyDone;
}

@end

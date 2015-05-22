#import <UIKit/UIKit.h>

#include <string>

@interface KeyboardView : UIView <UIKeyInput>

- (void) setText: (std::string)text;

@end

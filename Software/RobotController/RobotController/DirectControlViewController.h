//
//  DirectControlViewController.h
//  RobotController
//
//  Created by Sergey on 6/1/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol DirectControlScreenDelegate <NSObject>

- (void)closeButtonPressed:(UIButton *)button;
- (void)sliderSet:(UISlider *)slider;

@end

@interface DirectControlViewController : UIViewController <UIPickerViewDataSource, UIPickerViewDelegate>

@property (nonatomic, strong) UIButton *backButton;
@property (nonatomic, weak) NSObject <DirectControlScreenDelegate> *delegate;
@property (nonatomic) uint16_t minSliderValue;
@property (nonatomic) uint16_t maxSliderValue;

@end

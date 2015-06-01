//
//  DirectControlViewController.m
//  RobotController
//
//  Created by Sergey on 6/1/15.
//  Copyright (c) 2015 Sergey. All rights reserved.
//

#import "DirectControlViewController.h"

#define PICKER_CHANNELS_NUM             24

#define BACK_BUTTON_POSITION_CALC(S)    CGPointMake(S.width * 0.955f, S.height * 0.065f)

#define SLIDER_FRAME_CALC(S)            CGRectMake(0, 0, s.width * 0.7f, s.height * 0.1f)
#define SLIDER_POSITION_CALC(S)         CGPointMake(S.width * 0.5f, S.height * 0.8f)

#define LABEL_FRAME_CALC(S)             CGRectMake(0, 0, s.width * 0.5f, s.height * 0.1f)
#define LABEL_POSITION_CALC(S)          CGPointMake(S.width * 0.5f, S.height * 0.6f)

#define PICKER_FRAME_CALC(S)            CGRectMake(0, 0, s.width * 0.5f, s.height * 0.3f)
#define PICKER_POSITION_CALC(S)         CGPointMake(S.width * 0.5f, S.height * 0.2f)


@interface DirectControlViewController ()

@property (nonatomic, strong) UILabel *label;
@property (nonatomic, strong) UIPickerView *picker;
@property (nonatomic, strong) UISlider *slider;
@property (nonatomic, strong) NSArray *pickerData;
@property (nonatomic) uint8_t selectedChannel;

@end

@implementation DirectControlViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.view.backgroundColor = [UIColor orangeColor];
    
    CGSize s = self.view.frame.size;
    
    // Back button.
    self.backButton = [UIButton buttonWithType:UIButtonTypeCustom];
    UIImage *backBtnImg = [UIImage imageNamed:@"closeButton.png"];
    [self.backButton setImage:backBtnImg forState:UIControlStateNormal];
    self.backButton.frame = CGRectMake(0, 0, backBtnImg.size.width, backBtnImg.size.height);
    self.backButton.center = BACK_BUTTON_POSITION_CALC(s);
    [self.view addSubview:self.backButton];
    self.backButton.enabled = YES;
    [self.backButton addTarget:self action:@selector(backButtonPressed:)
                  forControlEvents:UIControlEventTouchUpInside];
    
    // Slider to control channel value.
    UISlider *slider = [[UISlider alloc] init];
    slider.frame = SLIDER_FRAME_CALC(s);
    slider.center = SLIDER_POSITION_CALC(s);
    slider.minimumValue = self.minSliderValue;
    slider.maximumValue = self.maxSliderValue;
    [self.view addSubview:slider];
    [slider addTarget:self action:@selector(sliderChanged:)
     forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];
    self.slider = slider;
    
    // Label to show value.
    UILabel * label = [[UILabel alloc] init];
    label.textAlignment = NSTextAlignmentCenter;
    label.font = [UIFont fontWithName:@"Optima" size:26];
    label.frame = LABEL_FRAME_CALC(s);
    label.center = LABEL_POSITION_CALC(s);
    label.text = [NSString stringWithFormat:@"%hu us", (uint16_t)slider.value];
    [self.view addSubview:label];
    self.label = label;
    
    // Channel number picker.
    UIPickerView *picker = [[UIPickerView alloc] initWithFrame:PICKER_FRAME_CALC(s)];
    picker.center = PICKER_POSITION_CALC(s);
    picker.delegate = self;
    picker.dataSource = self;
    picker.showsSelectionIndicator = NO;
    [self.view addSubview:picker];
    self.picker = picker;
    
    self.selectedChannel = 0;
}

- (void)backButtonPressed:(UIButton *)button
{
    if ([self.delegate respondsToSelector:@selector(closeButtonPressed:)])
    {
        [self.delegate closeButtonPressed:button];
    }
}

- (void)sliderChanged:(UISlider *)slider
{
    uint16_t value = (uint16_t)slider.value;
    
    self.label.text = [NSString stringWithFormat:@"%hu us", value];
    
    slider.tag = self.selectedChannel;
    
    if ([self.delegate respondsToSelector:@selector(sliderSet:)])
    {
        [self.delegate sliderSet:slider];
    }
}

#pragma mark - Picker delegate.

- (int)numberOfComponentsInPickerView:(UIPickerView *)pickerView
{
    return 1;
}

- (int)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component
{
    return PICKER_CHANNELS_NUM;
}

- (NSString*)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    return [NSString stringWithFormat:@"Channel %d", row + 1];
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    self.selectedChannel = row;
    
    [self sliderChanged:self.slider];
}

@end

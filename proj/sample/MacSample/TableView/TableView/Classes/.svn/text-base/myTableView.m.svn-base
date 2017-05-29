//
//  myTableView.m
//
//  Created by nishi on 10/08/25.
//  Copyright 2010 TARGET ENTERTAINMENT INC.. All rights reserved.
//

#import "myTableView.h"

@implementation myTableView

@synthesize selected;


-(void)setup
{
	self.delegate = self;
	self.dataSource = self;
	self.separatorColor = [UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:0.5];
}

// table = [[myTableView alloc] init] した場合コッチが呼ばれる
-(id)init
{
	if(self = [super init])
	{
		[self setup];
	}
	return self;
}

// 初期化。IBでパーツを配置した時はコッチが呼ばれる
-(id)initWithCoder:(NSCoder *)decoder
{
	if(self = [super initWithCoder:decoder])
	{
		[self setup];
	}
	return self;
}

-(void)dealloc
{
	self.selected = nil;
	self.delegate = nil;							// 先にdelegateを解除しておかないと、処理順番の都合でBAD_ACCESSになる事がある
	self.dataSource = nil;
	[super dealloc];
}


-(void)setCellColor:(UITableViewCell *)cell selected:(BOOL)aFlag
{
	cell.backgroundView.backgroundColor = aFlag ? [UIColor colorWithRed:0.0 green:0.0 blue:1.0 alpha:0.5] : [UIColor clearColor];
	cell.textLabel.textColor = aFlag ? [UIColor whiteColor] : [UIColor colorWithRed:0.8 green:0.8 blue:0.8 alpha:1.0];
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	return 20;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	UITableViewCell *cell = (UITableViewCell *)[tableView dequeueReusableCellWithIdentifier:@"test"];
	if(cell == nil)
	{
		cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:@"test"] autorelease];
		UIView *bgView = [[UILabel alloc] initWithFrame:CGRectZero];
		cell.backgroundView = bgView;				// ここでbgViewがretainされる
		[bgView release];							// autoreleaseではないので、自分でrelease

		// UIView *bgView = [[[UILabel alloc] initWithFrame:CGRectZero] autorelease];
		// cell.backgroundView = bgView;
		// ↑こう書いてもいいです
	}
	[self setCellColor:cell selected:([indexPath compare:selected] == NSOrderedSame)];

	UILabel *label = cell.textLabel;
 	label.text = [NSString stringWithFormat:@"test%02d", indexPath.row];
	label.backgroundColor = [UIColor clearColor];

	return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	// テーブルの再読み込みはちょっと勿体ないと思ったので、必要最低限の書き替えにしてみました
	if(selected != nil)
	{
		UITableViewCell *cell = [tableView cellForRowAtIndexPath:selected];
		[self setCellColor:cell selected:NO];
	}

	{
		UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
		[self setCellColor:cell selected:YES];
		[tableView deselectRowAtIndexPath:indexPath animated:YES];
		self.selected = indexPath;
		// ↑アクセサメソッドを自動生成しているので、上のコードでは、
		// 先に先に入っているオブジェクトをreleaseしてから新しいオブジェクトをretainしてくれる
	}
}

@end

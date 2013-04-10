//
//  MyTableView.m
//
//  Created by IKEDA KOHEI on 10/08/24.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MyTableView.h"

@implementation MyTableView

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	return 14;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	
    static NSString *CellIdentifier = @"Cell";
	
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithFrame:CGRectZero reuseIdentifier:CellIdentifier] autorelease];
    }
	
	// Set up the cell...
///	cell.textLabel.text = @"TEST";
	UILabel* label = cell.textLabel;
	label.text = @"TEST";
    return cell;
}
@end

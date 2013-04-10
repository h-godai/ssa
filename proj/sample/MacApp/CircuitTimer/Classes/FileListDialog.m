//
//  FileListDialog.m
//  CircuitTimer
//
//  Created by IKEDA KOHEI on 10/10/01.
//  Copyright 2010 SoftwareSoietyAkihabara. All rights reserved.
//

#import "FileListDialog.h"
#import "ConfigViewController.h"

@implementation FileListDialog
@synthesize popoverController_;


#pragma mark -
#pragma mark View lifecycle


- (void)viewDidLoad {
  int i;
  [super viewDidLoad];

  // Uncomment the following line to preserve selection between presentations.
  // self.clearsSelectionOnViewWillAppear = NO;
  
  // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
  // self.navigationItem.rightBarButtonItem = self.editButtonItem;
  
  

  NSString* dirPath = [NSString stringWithFormat: @"%@/Documents", NSHomeDirectory()]; //[openPanel filename];
  NSArray* dir = [[NSFileManager defaultManager] directoryContentsAtPath:dirPath];
  dirArray_ = [dir mutableCopy];
  [dirArray_ retain];

  for (i = 0; i < dirArray_.count; ++i) {
    // 拡張子が".log"以外を排除
    NSString* str = (NSString*)[dirArray_ objectAtIndex: i];
    if ([str length] < 5 
	|| [[str substringFromIndex: [str length] - 4] compare: @".log"] != NSOrderedSame) {
      // ".log"ではない
      [dirArray_ removeObjectAtIndex: i];
      --i;
    }
  }
  //[dirArray_ insertObject: @"<<新規作成>>" atIndex: 0];
  NSLog(@"%@\n", [dirArray_ description]);

}

/*
- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
}
*/
/*
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}
*/
/*
- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
}
*/
/*
- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
}
*/


- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Override to allow orientations other than the default portrait orientation.
    return YES;
}


#pragma mark -
#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    // Return the number of sections.
  return 1; // <#number of sections#>;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    // Return the number of rows in the section.
  return [dirArray_ count]; //;<#number of rows in section#>;
}


// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
  
  static NSString *CellIdentifier = @"Cell";
  
  UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
  if (cell == nil) {
    cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:CellIdentifier] autorelease];
  }

  CTRaceInfo rinfo;
  int laps = [MCircuitTimer loadRaceInfo: &rinfo 
			    filename: [self getFileName: indexPath.row]];
  if (laps >= 0) {
    // Configure the cell...
    cell.textLabel.text = [NSString stringWithFormat: @"%04d/%02d/%02d %02d:%02d %03dLaps %@"
				    , rinfo.raceDate.year_
				    , rinfo.raceDate.month_
				    , rinfo.raceDate.day_
				    , rinfo.raceDate.hour_
				    , rinfo.raceDate.min_
				    , laps
				    , rinfo.raceName];
  }
  else {
    cell.textLabel.text = [NSString stringWithFormat: @"Load Faild:%@",
				    [dirArray_ objectAtIndex: indexPath.row]];
  }
  
  return cell;
}



// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}




// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        //[tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:YES];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}



/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath {
}
*/


/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/


#pragma mark -
#pragma mark Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
  // Navigation logic may go here. Create and push another view controller.
  /*
    <#DetailViewController#> *detailViewController = [[<#DetailViewController#> alloc] initWithNibName:@"<#Nib name#>" bundle:nil];
    // ...
    // Pass the selected object to the new view controller.
    [self.navigationController pushViewController:detailViewController animated:YES];
    [detailViewController release];
  */
  [tableView deselectRowAtIndexPath:indexPath animated:YES];

  [(ConfigViewController*)popoverController_.delegate 
			  onSelectFile: [self getFileName:indexPath.row]];
}

- (NSString*)getFileName: (int)index {
  return [NSString stringWithFormat: @"%@/Documents/%@"
		   , NSHomeDirectory()
		   , [dirArray_ objectAtIndex: index]];
}


#pragma mark -
#pragma mark Memory management

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Relinquish ownership any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    // Relinquish ownership of anything that can be recreated in viewDidLoad or on demand.
    // For example: self.myOutlet = nil;
}


- (void)dealloc {
  [super dealloc];
}


@end


//
//  ViewController.swift
//  ServerAdmin
//
//  Created by Jonathan Cichon on 02.06.17.
//  Copyright © 2017 pixelflut GmbH. All rights reserved.
//

import Cocoa
import WebKit

class ViewController: NSSplitViewController {
    
    var webController: WebController?

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        webController = WebController();
        
        let splitViewItem = NSSplitViewItem(viewController: webController!)
        splitViewItem.canCollapse = false
        self.addSplitViewItem(splitViewItem)
        
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    override func viewDidAppear() {
//        let splitViewItem = self.splitViewItem(for: webController!)
//        
//        splitViewItem?.viewController.view.frame = CGRect(x:0, y:0, width: (self.view.window?.frame.width)!, height: (self.view.window?.frame.height)!);
//        print(splitViewItem?.viewController.view.frame as Any)
        
    }


}


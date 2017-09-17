//
//  TerminalViewController.swift
//  ServerAdmin
//
//  Created by Jonathan Cichon on 13.06.17.
//  Copyright © 2017 pixelflut GmbH. All rights reserved.
//

import Cocoa

class TerminalViewController: NSViewController {
    
    @IBOutlet weak var textView: NSTextView!

    override func viewDidLoad() {
        super.viewDidLoad()
        self.textView.isEditable = false
        NotificationCenter.default.addObserver(self, selector: #selector(TerminalViewController.updateText), name: NSNotification.Name.SessionDidUpdateOutput, object: nil)
    }
    
    func updateText(_ aNotification: Notification) {
        DispatchQueue.main.async {
            let outputString = aNotification.object as! String
            let previousOutput = self.textView.string ?? ""
            let nextOutput = previousOutput + "\n" + outputString
            self.textView.string = nextOutput
            
            let range = NSRange(location:nextOutput.characters.count,length:0)
            self.textView.scrollRangeToVisible(range)
        }
        
    }
    
}

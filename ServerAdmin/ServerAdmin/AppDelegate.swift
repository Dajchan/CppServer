//
//  AppDelegate.swift
//  ServerAdmin
//
//  Created by Jonathan Cichon on 02.06.17.
//  Copyright © 2017 pixelflut GmbH. All rights reserved.
//

import Cocoa

extension NSNotification.Name {
    public static let SessionDidChange: NSNotification.Name = NSNotification.Name(rawValue: "SessionDidChange")
}


@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    var sharedSession: Session? {
        didSet(session) {
            print("didSet")
            print(sharedSession ?? "no session")
            NotificationCenter.default.post(name: NSNotification.Name.SessionDidChange, object: sharedSession)
        }
    }
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        
        print(Bundle.main.bundleURL)
        
        // load sharedSession
        if (sharedSession == nil) {
            let alert = SessionSetupAlert()
            alert.messageText = "messageText"
            alert.informativeText = "informativeText"
            
            alert.beginSheetModal(for: NSApplication.shared().windows.first!, completionHandler: { (modal) in
                self.sharedSession = Session(rootServerURL: URL(string: alert.localURL.stringValue)!,
                                             rootFilePath: alert.localPath.stringValue,
                                             remoteServerURL: URL(string: alert.remoteURL.stringValue)!,
                                             remoteServerUser: alert.remoteUser.stringValue,
                                             remoteServerPassword: alert.remotePassword.stringValue)
            })
        }
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
        print("shut down server")
        if self.sharedSession != nil {
            self.sharedSession?.process.stop()
            print("server shut down")
        }
    }
    
    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }


}


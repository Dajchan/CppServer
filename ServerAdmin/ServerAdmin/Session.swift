//
//  Session.swift
//  ServerAdmin
//
//  Created by Jonathan Cichon on 06.06.17.
//  Copyright © 2017 pixelflut GmbH. All rights reserved.
//

import Cocoa


extension NSNotification.Name {
    public static let SessionDidUpdateOutput: NSNotification.Name = NSNotification.Name(rawValue: "SessionDidUpdateOutput")
}

class Session: NSObject {
    let rootServerURL: URL
    let rootFilePath: String
    
    let remoteServerURL: URL
    let remoteServerUser: String
    let remoteServerPassword: String
    
    let process: ShellCmd
    
    init(rootServerURL: URL, rootFilePath: String, remoteServerURL: URL, remoteServerUser: String, remoteServerPassword: String) {
        self.rootServerURL = rootServerURL
        self.rootFilePath = rootFilePath
        
        self.remoteServerURL = remoteServerURL
        self.remoteServerUser = remoteServerUser
        self.remoteServerPassword = remoteServerPassword
        self.process = ShellCmd(args: [self.rootFilePath + "/webserver", "3001", self.rootFilePath + "/website"])
        
        self.process.progressHandler = {(newString) in
            NotificationCenter.default.post(name: NSNotification.Name.SessionDidUpdateOutput, object: newString)
//            print(newString)
        }
        
        self.process.runInBackground { (status) in
            print(status)
        }
        
        super.init()
    }
    
}

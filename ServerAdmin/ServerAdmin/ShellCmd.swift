//
//  ShellCmd.swift
//  ServerAdmin
//
//  Created by Jonathan Cichon on 08.06.17.
//  Copyright © 2017 pixelflut GmbH. All rights reserved.
//

import Cocoa

class ShellCmd: NSObject {
    
    private let args: [String]
    private var process: Process?
    private var pipe: Pipe?
    var progressHandler: ((String) -> Swift.Void)?
    
    init(args: [String]) {
        self.args = args
        super.init()
    }
    
    func receivedData(notification: Notification) {
        print(notification)
        let fh = notification.object as! FileHandle
        let output = fh.availableData
        
        if output.count > 0 {
            fh.waitForDataInBackgroundAndNotify()
            if self.progressHandler != nil {
                
                let outputString = String(data: output, encoding: String.Encoding.utf8) ?? ""
                
                print(outputString)
                self.progressHandler!(outputString)
            }
            
        } else {
            DispatchQueue.global(qos: .background).asyncAfter(deadline: DispatchTime.now() + Double(Int64(0.1 * Double(NSEC_PER_SEC))) / Double(NSEC_PER_SEC), execute: {
                fh.waitForDataInBackgroundAndNotify()
            })
        }
    }
    
    func run() -> Int32 {
        let p = Process()
        process = p
        
        p.launchPath = "/usr/bin/env"
        p.arguments = args
        
        pipe = Pipe()
        p.standardOutput = pipe
        pipe?.fileHandleForReading.readabilityHandler = {
            fh in
            let output = fh.availableData
            
            if output.count > 0 {
                if self.progressHandler != nil {
                    
                    let outputString = String(data: output, encoding: String.Encoding.utf8) ?? ""
                    
                    print(outputString)
                    self.progressHandler!(outputString)
                }
            }
        }
        
        p.launch()
        p.waitUntilExit()
        pipe?.fileHandleForReading.readabilityHandler = nil
        
        return p.terminationStatus
    }
    
    func runInBackground(completionBlock: @escaping (_ terminationStatus: Int32) -> Swift.Void) -> Swift.Void {
        weak var selfWeak = self
        DispatchQueue.global(qos: .background).async {
            let selfStrong = selfWeak
            if selfStrong != nil {
                let status = selfStrong!.run()
                completionBlock(status)
            }
            
        }
    }
    
    func stop() {
        process!.terminate()
    }
    
    deinit {
        if process!.isRunning {
            process!.terminate()
        }
    }
}

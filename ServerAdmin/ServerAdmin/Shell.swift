//
//  Shell.swift
//  ServerAdmin
//
//  Created by Jonathan Cichon on 08.06.17.
//  Copyright © 2017 pixelflut GmbH. All rights reserved.
//

import Foundation

@discardableResult
func shell(_ args: String...) -> Int32 {
    let task = Process()
    task.launchPath = "/usr/bin/env"
    task.arguments = args
    task.launch()
    task.waitUntilExit()
    return task.terminationStatus
}

func execute_shell(_ args: String..., using block: (_ newData: String?) -> Swift.Void) -> Int32 {
    // Create a Task instance
    let task = Process()
    
    // Set the task parameters
    task.launchPath = "/usr/bin/env"
    task.arguments = args
    
    // Create a Pipe and make the task
    // put all the output there
    let pipe = Pipe()
    task.standardOutput = pipe
    
    // Launch the task
    task.launch()
    task.waitUntilExit()
    return task.terminationStatus
    
//    // Get the data
//    let data = pipe.fileHandleForReading.readDataToEndOfFile()
//    let output = NSString(data: data, encoding: String.Encoding.utf8.rawValue)
//    
//    print(output!)
//    return ""
}

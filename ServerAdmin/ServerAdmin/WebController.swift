//
//  WebController.swift
//  ServerAdmin
//
//  Created by Jonathan Cichon on 02.06.17.
//  Copyright © 2017 pixelflut GmbH. All rights reserved.
//

import WebKit

class WebController: NSViewController, WKNavigationDelegate {
    
//    override var view: WKWebView?
    var webView: WKWebView {
        return self.view as! WKWebView
    }
    
    override func loadView() {
        let v = WKWebView()
        v.navigationDelegate = self
        view = v
        v.addConstraint(NSLayoutConstraint(item: v, attribute: NSLayoutAttribute.width, relatedBy: NSLayoutRelation.greaterThanOrEqual, toItem: nil, attribute: NSLayoutAttribute.notAnAttribute, multiplier: 1, constant: 500))
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        NotificationCenter.default.addObserver(self, selector: #selector(WebController.reloadRoot), name: NSNotification.Name.SessionDidChange, object: nil)
    }
    
    override var representedObject: Any? {
        didSet {
            // Update the view, if already loaded.
        }
    }
    
    override func viewDidAppear() {
        self.webView.load(URLRequest(url: URL(string: "http://www.google.com")!))
    }
    
    func reloadRoot(notification: Notification) {
        if (notification.object != nil) {
            let session = notification.object as! Session
            self.webView.load(URLRequest(url: session.rootServerURL))
        }
        
    }
    
    deinit {
        NotificationCenter.default.removeObserver(self)
    }
    
// MARK: WKNavigationDelegate
    
    func webView(_ webView: WKWebView, didFail navigation: WKNavigation!, withError error: Error) {
        print(error)
    }
    
    func webView(_ webView: WKWebView, didFinish navigation: WKNavigation!) {
        print("didFinish")
    }
    
//    func webView(_ webView: WKWebView, decidePolicyFor navigationAction: WKNavigationAction, decisionHandler: @escaping (WKNavigationActionPolicy) -> Void) {
//        print("decidePolicyFor navigationAction")
//        decisionHandler(WKNavigationActionPolicy.allow)
//    }
//    
//    func webView(_ webView: WKWebView, didStartProvisionalNavigation navigation: WKNavigation!) {
//        print("didStartProvisionalNavigation")
//    }
//    
//    func webView(_ webView: WKWebView, didFailProvisionalNavigation navigation: WKNavigation!, withError error: Error) {
//        print("didFailProvisionalNavigation" + error.localizedDescription)
//    }
    
}

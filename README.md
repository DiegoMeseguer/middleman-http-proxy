Student: Diego Meseguer  
Assignment: Middleman Proxy

### Instructions
- Use with Firefox
    - To configure Firefox:
    - Click on the sandwich menu on the upper right corner
    - Click on settings
    - Scroll all the way to the bottom of the General selection
    - In the Network Settings subsection, click on the Settings... button
    - Select the option that says "Manual proxy configuration"
    - In the HTTP Proxy form, type "127.0.0.1"
    - In the Port form next to the HTTP Proxy form, type any port between 1024 and 65535. For example 7005
    - Click OK
    - Go to the Privacy & Security section
    - Scroll down to the Cookies and Site Data subsection
    - Click the Clear Data... button
    - Make sure that all the options are selected
    - Click the Clear button
    - If Firefox shows a confirmation prompt, click the Clear Now button
- Open a terminal
- Go to the same directory where the proxy.c file is
- Run the command "clang proxy.c -o proxy" or "gcc proxy.c -o proxy"
- The program needs 1 argument which is a port number, this should be the same port that was configured in Firefox
- So if we used port 7005, then we run "./proxy 7005"
- If there is any prompt about allowing the proxy to accept incoming connections we accept/allow
- Once the proxy is running, we should see the message "Listening to incoming connections..."
- Now we can go to Firefox and try a website like http://neverssl.com/
- The proxy may take a few seconds to completely send all the responses to Firefox
- When the proxy is ready to process a new site we will see the message "Waiting for a new request from the browser"
- Now we can try a new website, like httpforever.com/
- When we want to end the proxy, we press "control + c" in the terminal

Websites that work:  
http://neverssl.com/  
http://forever.com/  
Websites that work, but proxy gets stuck at the end of the loop:  
http://www.slackware.com/  
http://www.columbia.edu/~fdc/sample.html


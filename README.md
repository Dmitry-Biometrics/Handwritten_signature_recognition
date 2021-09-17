The work is devoted to mastering the technology of converting a person's biometric data into the code of his personal access key or long password. The work will be introduced in the program "BioNeuroAutograph 2.0" (Figure 1).

![image](https://user-images.githubusercontent.com/16018075/133831504-2419fd23-a3b0-4b4b-949f-27050f543e4f.png)

    Figure 1 - The interface of the program "BioNeuroAutograph 2.0"

In the sub-item "mode / set user password" I have created a login and password (Figure 2) for further training and testing of the system.

![image](https://user-images.githubusercontent.com/16018075/133831547-5345a072-acd1-43b3-9c33-76bb80097c3d.png)

    Figure 2 - Creating a new user

Then, using the mouse, 12 images of the letter "a" were entered (Figure 3).

![image](https://user-images.githubusercontent.com/16018075/133831614-3152c20b-9405-44d2-b77c-44328f62652a.png)

    Figure 3 - Input of images for training a neural network

After clicking on the "Train the network" button, the training process began and a window with the training results appeared - Figure 4.

![image](https://user-images.githubusercontent.com/16018075/133831647-220da0c7-fc40-4748-8b1f-4aa75246ebb0.png)

    Figure 4 - The result of training a neural network

Upon completion of training the neural network, I checked the rewritten letter "a". When you click on the "Check" button, the following dialog box was displayed, shown in Figure 5.

![image](https://user-images.githubusercontent.com/16018075/133831679-ab89fb40-91eb-4107-b1a4-9faaaf5ebcc1.png)

    Figure 5 - The result of checking the re-entered image

Since I entered a 5-digit password "aaaaa", the program outputted a code consisting of 40 bits - a sequence of zeros and ones. One character corresponds to 8 bits. To increase the security, I generated a new password consisting of 32 characters, which corresponds to 256 bits.
The goal of BioNeuroAutograph 2.0 is to show the user how a biometric authentication system can be used instead of memorizing a long password. Based on the results of the work done, the user just needs to remember the password, which was written down by his hand, using the mouse (letter "a").
I retrained with a new password. When I pressed the "Test" button, I re-wrote the letter "a". When you click on the "Check" button, the program identified me. The green traffic light came on. The new password, consisting of 32 characters, was repeated completely error-free.

As a check, the letter "a" was written by a stranger and submitted for verification. An outsider does not know the peculiarities of my handwriting, and as a result, the program recognized it as "alien" (Figure 7). The red traffic light came on. When other characters were introduced (letters "c", "b", "d"), the program accordingly also identified it as an "incorrect password".

![image](https://user-images.githubusercontent.com/16018075/133831739-d7e8e268-af3c-4915-a039-3909826ce690.png)

    Figure 7 - An example of identification of an unauthorized user

![image](https://user-images.githubusercontent.com/16018075/133831762-fbbda426-4203-47ee-bbd0-c7bc897e6d05.png)
    Figure 8 - An example of mismatched bits when entering another image



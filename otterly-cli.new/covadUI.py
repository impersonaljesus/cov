# Import the necessary packages
from cursesmenu import *
from cursesmenu.items import *
from time import sleep
from os import system
#from pynput.keyboard import Key, Listener

# ~ def show(key):
  
    # ~ print('\nYou Entered {0}'.format( key))
  
    # ~ if key == Key.delete:
        # ~ # Stop listener
        # ~ return False

def covidTest():
	system("/home/pi/cov/otterly-cli.new/Otterly-CCD-CLI")
	print("returning to menu in 10 seconds")
	sleep(10)
	return

	

def sysShutdown():
	print("The COVAD unit will begin shutdown in 5 seconds")
	sleep(5)
	os.system("sudo shutdown now")
	
	
# Create the menu
menu = CursesMenu("COVA Testing Unit", "Main Menu")

# Create some items

# A FunctionItem runs a Python function when selected
function_item = FunctionItem("Run a COVID test", covidTest)

# A FunctionItem runs a Python function when selected
function_item2 = FunctionItem("Shutdown the unit", sysShutdown)

# Once we're done creating them, we just add the items to the menu
menu.append_item(function_item)
menu.append_item(function_item2)

# Finally, we call show to show the menu and allow the user to interact
menu.show()

#-
#  GROUPE : Alan Dabrowski (dabrowskia@cy-tech.fr) - Gaspard Pepin (pepingaspa@cy-tech.fr)
#  FICHIER : COMPILATEUR.PY
#  EXECUTION : python3 compilateur.py <nom.s>
#-

#IMPORT
import os
import sys
import struct
import argparse
from string import punctuation
from types import NoneType

#GLOBAL VARIABLES
branche = {"B" : 0x8,"BEQ" : 0x9,"BNE" : 0xa,"BLE" : 0xb,"BGE" : 0xc,"BL" : 0xd,"BG" : 0xe}
OPCodes = {"AND" : [0,3], "ORR" : [1,3], "EOR" : [2,3], "ADD" : [3,3], "ADC" : [4,3], "CMP" : [5,2], "SUB" : [6,3], "SBC" : [7,3], "MOV" : [8,2], "LSH" : [9,3], "RSH" : [10,3]}


#-
#  FUNCTION : OUVERTURE
#  PARAMETER : [nom - filename to be open]
#  GOAL : Opens the file - Reads it into res - Returns res
#-
def ouverture(nom) :

    #Verification for namefile
    if(type(nom) == NoneType):
        print("ERROR : \nNo filename given. Please give a path or a filename as :\npython3 compilateur.py -f <filename>")
        sys.exit()
    try :
        #File opening in read mode
        file = open(nom,'r')
    except OSError :
        print("ERROR : \nCould not find the file "+nom+".\nPlease verify the filename or the path.")
        sys.exit()

    #File reading
    res = file.readlines()

    #File closing
    file.close()

    #Return res
    return(res)

#-
#  FUNCTION : CONVERSION
#  PARAMETER : [OPCode - assembly of the instruction] - [dest - destination of the instruction] - [ope1 - first operand] - [ope2 - second operand] - [jump - boolean indicating jump or not]
#  GOAL :  Converts the given instructions into a number from 0 to 2⁶⁴-1 - Returns this number as res
#-
def conversion(OPCode,dest,ope1,ope2,jump):
    res = 0
    count = 0
    #Counting the number of arguments to the asm instruction
    if dest != "": count += 1
    if ope1 != "": count += 1
    if ope2 != "": count += 1

    if jump :
        #In the case of the wrong number of arguments
        if count != 1:
            print("Wrong number of arguments for BRANCH function. 1 argument expected, "+str(count)+" given.")
            return(-1)

        #Calculing result address for the jump
        res += 2**28*branche[OPCode]
        temp = int(dest)
        if temp<0:
            res += 2**27
            temp = -temp
        res += temp

    else :
        dest = dest[1:]
        #In the case of the wrong number of arguments
        if count != OPCodes[OPCode][1]:
            print("Wrong number of arguments for "+OPCode+". Arguments expected : "+str(OPCodes[OPCode][1])+", given : "+str(count)+".")
            return(-1)

        res += 2**20*OPCodes[OPCode][0]
        if OPCode == "CMP":
            res += 2**16*int(dest)
        else:
            res += 2**8*int(dest)
        # ope2 => IV if not a registre
        if ope2[0]!= "R":
            res += 2**24
            res += int(ope2)
        else:
            ope2 = ope2[1:]
            res += 2**12*int(ope2)

        if count == 3:
            res += 2**16*int(ope1[1:])

    return(res)

#-
#  FUNCTION : TRADUCTION
#  PARAMETER : [liste - list of instructions from the file]
#  GOAL : Gets all the instructions as a char list - turns them into instructions readable by conversion - returns the list of instructions to put in the binary file as numbers
def traduction(liste) :
    res = []

    #Creation of punction list that will be deleted from instructions
    aSupp = ""
    for i in punctuation:
        if i != ";" and i != "-":
            aSupp += i
    
    #Deleting \n and setting all in caps
    for i in range(len(liste)) :
        liste[i] = liste[i].upper()
        liste[i] = list(liste[i])
        if liste[i][len(liste[i])-1] == "\n":
            liste[i].pop()

    #Going line by line in instructions - stocking result in new list
    for i in liste :
        prevSpace = False
        ret = 0
        jump = False
        arg = ""
        dest = ""
        ope2 = ""
        ope1 = ""
        count = 0
        comment = False
        #Going character by character in the line
        for j in range(len(i)):
            i = list(i)
            temp = i.pop(0)

            #Resetting the variable for following spaces
            if temp != " " and prevSpace:
                prevSpace = False

            #Comment
            if temp == ";": 
                comment = True

            #Deleting useless characters
            if temp in aSupp: 
                temp = " "
            
            #Dealing with comment
            if comment:()

            #Dealing with two following spaces
            elif temp == " " and prevSpace:()

            #Merging characters in arg
            elif count == 0 and temp != " ":
                arg+=temp
            
            #Passing to first argument after instruction / dealing with jump
            elif count == 0 and temp == " ":
                count += 1
                prevSpace = True
                if arg in branche :
                    count == -1
                    jump = True

            #Finding destination of jump
            elif count == -1:
                dest += temp

            #Merging characters for destination
            elif count == 1 and temp != " ":
                dest += temp

            #Passing to argument 1
            elif count == 1 and temp == " ":
                count += 1
                prevSpace = True

            #Merging characters for argument 1
            elif count == 2 and temp != " ":
                ope2 += temp

            #Passing to argument 2
            elif count == 2 and temp == " ":
                count  += 1
                prevSpace = True
                if i!=[] and OPCodes[arg][1] != 2:
                    ope1 = ope2
                    ope2 = ""

            #Merging characters for argument 2
            elif count == 3 and temp != " ":
                ope2 += temp

            #Ending compilation if too many arguments
            elif count == 3 and prevSpace:
                print("Error : \nToo many given arguments. \nExpected for "+arg+" : "+str(OPCodes[arg][1]))
                exit()
        #Conversion of the current instructions
        if arg != "":
            res.append(conversion(arg,dest,ope1,ope2,jump))

    return(res)

#-
#  FUNCTION : ECRITURE
#  PARAMETER : [name - name of the bin to create] [liste -  list with all instructions of the file]
#  GOAL : Launches the translation - Writes translated bytes into a bin file
#-
def ecriture(name, liste):
    
    #Traduction of the instruction list
    print("Instructions translation...")
    res = traduction(liste)

    #Writing binary in file (BigEndian)
    print("Writing in bin file...")
    with open(name+".bin","wb") as file:
        for i in res:
            file.write(struct.pack(">I", i))

#-
#  FUNCTION : main
#  PARAMETER : NONE
#  GOAL : Main function of program - Opens the asm file - Translates it then writes it in a bin file
#-
def main():

    #Getting arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file", type=str)
    instr = parser.parse_args().file

    #Opening file
    print("File opening.")
    lines = ouverture(instr)

    #Compilation
    print("Bin creation.")
    ecriture( os.path.splitext(instr)[0],lines)


#Call to main function
main()
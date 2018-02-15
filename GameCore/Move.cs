using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameCore
{

   
    public class Move
    {
       public int row;
       public char col;
       public COORD Begin = new COORD();
       public COORD End = new COORD();

        public void convertMove(COORD coord)
        {
            coord.X = row - 1;
            coord.Y = col - 64 - 1;
        }

        public void getFromUser(Player currentPlayer)
        {
            do
            {
                getBeginMove();
            }
            while (!Program.checkCOORD(currentPlayer, this));

            do
            {
                getEndMove();
            }
            while (!Program.checkMove(currentPlayer, this));
        }
 
        private void getBeginMove()
        {
            Console.Write("\n");
            Console.Write("What COORD do you want to move?");
            Console.Write("\n");
            col = Console.ReadKey().KeyChar;
            row = (int)Console.ReadKey().KeyChar - 48;
            Console.Write("\n");
            convertMove(Begin);

        }

        private void getEndMove()
        {
            Console.Write("Where do you want to move");
            Console.Write("?\n");
            col = Console.ReadKey().KeyChar;
            row = (int)Console.ReadKey().KeyChar - 48;
            convertMove(End);
            Console.Write("\n");
        }



    }




}

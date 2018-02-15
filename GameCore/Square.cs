using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameCore
{
    public class Square
    {
        public static Square X = new Square('X');
        public static Square O = new Square('O');
        public static Square S = new Square('S');

        public static Square fromIdentity(identity id)
        {
            if (id == identity.X)
            {
                return Square.X;
            }
            else
            {
                return Square.O;
            }
        }

        public static Square fromOppositeIdentity(identity id)
        {
            if (id == identity.O)
            {
                return Square.X;
            }
            else
            {
                return Square.O;
            }
        }


        //Non static members
        private char character;

        private Square(char c)
        {
            character = c;
        }
        public static explicit operator char(Square s)
        {
            return s.character;
        }



    }
}

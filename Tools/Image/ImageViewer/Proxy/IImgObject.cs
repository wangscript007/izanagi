﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ImageViewer
{
    public interface IImgObject : IDisposable
    {
        IImgObject Parent
        {
            get;
        }

        int ID
        {
            get;
        }
    }
}
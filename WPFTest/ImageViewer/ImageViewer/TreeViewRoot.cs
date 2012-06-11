﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using ImageViewerInfra;

namespace ImageViewer
{
    /// <summary>
    /// ツリービューのルート
    /// </summary>
    class TreeViewRoot : ObservableCollection<ImageTreeViewNode>
    {
        private static TreeViewRoot instance = new TreeViewRoot();

        public static TreeViewRoot Instance
        {
            get
            {
                return instance;
            }
        }

        private TreeViewRoot()
        {
        }

        public void Register(ImgMaster imgMaster)
        {
            ImageTreeViewNode node = new ImageTreeViewNode(imgMaster, imgMaster.Name);
            this.Add(node);

            int texCnt = 0;
            foreach (var tex in imgMaster.GetTexture())
            {
                string texName = "Texture_" + texCnt;
                var texNode = new ImageTreeViewNode(tex, texName);
                node.Children.Add(texNode);

                int imgCnt = 0;
                foreach (var img in tex.GetImage())
                {
                    string imgName = "MipMap_" + imgCnt;
                    var imgNode = new ImageTreeViewNode(img, imgName);
                    texNode.Children.Add(imgNode);
                }
            }
        }
    }
}
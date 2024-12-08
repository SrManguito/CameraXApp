package com.example.cameraxapp

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class MethodAdapter(
    private val methodList: List<MethodItem>,
    private val clickListener: (MethodItem) -> Unit
) : RecyclerView.Adapter<MethodAdapter.MethodViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MethodViewHolder {
        val view = LayoutInflater.from(parent.context).inflate(R.layout.item_method, parent, false)
        return MethodViewHolder(view)
    }

    override fun onBindViewHolder(holder: MethodViewHolder, position: Int) {
        holder.bind(methodList[position], clickListener)
    }

    override fun getItemCount(): Int = methodList.size

    class MethodViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        fun bind(item: MethodItem, clickListener: (MethodItem) -> Unit) {
            itemView.findViewById<ImageView>(R.id.methodIcon).setImageResource(item.imageResId)
            itemView.findViewById<TextView>(R.id.methodName).text = item.name
            itemView.findViewById<TextView>(R.id.methodDescription).text = item.description
            itemView.setOnClickListener { clickListener(item) }
        }
    }
}


